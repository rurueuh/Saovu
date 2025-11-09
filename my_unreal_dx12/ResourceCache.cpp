#include "ResourceCache.h"
#include "Mesh.h"
#include "windowDX12.h"
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <DirectXMath.h>
#include <iostream>


struct Material {
    DirectX::XMFLOAT3 Kd{ 1,1,1 };
    std::string map_Kd;
};

static std::string joinPath(const std::string& a, const std::string& b) {
    if (a.empty()) return b;
    const char last = a.back();
    if (last == '/' || last == '\\') return a + b;
    return a + "/" + b;
}

static void parseMtlFile(const std::string& mtlPath, std::unordered_map<std::string, Material>& out) {
    std::ifstream mtl(mtlPath);
    if (!mtl.is_open()) {
        std::cerr << "MTL introuvable: " << mtlPath << "\n";
        return;
    }

    std::string line, tok, cur;
    while (std::getline(mtl, line)) {
        std::istringstream iss(line);
        if (!(iss >> tok)) continue;
        if (tok == "newmtl") {
            iss >> cur; out[cur] = Material{};
        }
        else if (tok == "Kd" && !cur.empty()) {
            iss >> out[cur].Kd.x >> out[cur].Kd.y >> out[cur].Kd.z;
        }
        else if (tok == "map_Kd" && !cur.empty()) {
            iss >> out[cur].map_Kd;
        }
    }
}

static void parseVtxToken(const std::string& tok, int& vi, int& ti, int& ni) {
    vi = ti = ni = 0;
    int part = 0;
    std::string acc;
    auto flush = [&](void) {
        if (acc.empty()) { ++part; return; }
        int val = std::stoi(acc);
        if (part == 0) vi = val;
        else if (part == 1) ti = val;
        else if (part == 2) ni = val;
        acc.clear(); ++part;
        };
    for (char c : tok) { if (c == '/') flush(); else acc.push_back(c); }
    flush();
}

static uint16_t getIndexForKey(
    const std::string& key,
    std::unordered_map<std::string, uint16_t>& map,
    uint16_t& next,
    std::vector<Vertex>& outVertices,
    const std::vector<DirectX::XMFLOAT3>& positions,
    const std::vector<DirectX::XMFLOAT2>& texcoords,
    const std::vector<DirectX::XMFLOAT3>& normals)
{
    auto it = map.find(key);
    if (it != map.end()) return it->second;

    int vi = 0, ti = 0, ni = 0;
    parseVtxToken(key, vi, ti, ni);

    Vertex vert{};
    const auto& p = positions[(vi > 0 ? vi - 1 : 0)];
    vert.px = p.x; vert.py = p.y; vert.pz = p.z;
    vert.r = vert.g = vert.b = 1.0f;

    if (ti > 0) {
        const auto& t = texcoords[ti - 1];
        vert.u = t.x; vert.v = 1.0f - t.y;
    }
    else {
        vert.u = vert.v = 0.0f;
    }

    outVertices.push_back(vert);
    map[key] = next;
    return next++;
}

void LoadOBJIntoAsset(const std::string& filename, MeshAsset& out, std::shared_ptr<Texture> defaultWhite)
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: unable to open " << filename << std::endl;
        return;
    }

    const size_t slash = filename.find_last_of("/\\");
    const std::string baseDir = (slash == std::string::npos) ? "" : filename.substr(0, slash + 1);

    std::vector<DirectX::XMFLOAT3> positions;
    std::vector<DirectX::XMFLOAT3> normals;
    std::vector<DirectX::XMFLOAT2> texcoords;
    std::unordered_map<std::string, Material> materials;

    std::unordered_map<std::string, uint16_t> vertexMap;
    uint16_t nextIndex = 0;

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string type;
        iss >> type;

        if (type == "v") {
            DirectX::XMFLOAT3 pos;
            iss >> pos.x >> pos.y >> pos.z;
            positions.push_back(pos);
        }
        else if (type == "vt") {
            DirectX::XMFLOAT2 tex;
            iss >> tex.x >> tex.y;
            texcoords.push_back(tex);
        }
        else if (type == "vn") {
            DirectX::XMFLOAT3 n;
            iss >> n.x >> n.y >> n.z;
            normals.push_back(n);
        }
        else if (type == "f") {
            std::vector<std::string> toks;
            std::string tok;
            while (iss >> tok) toks.push_back(tok);
            if (toks.size() < 3) continue;

            std::vector<uint16_t> faceIdx;
            faceIdx.reserve(toks.size());
            for (auto& t : toks) {
                uint16_t idx = getIndexForKey(t, vertexMap, nextIndex, out.vertices, positions, texcoords, normals);
                faceIdx.push_back(idx);
            }
            for (size_t i = 2; i < faceIdx.size(); ++i) {
                out.indices.push_back(faceIdx[0]);
                out.indices.push_back(faceIdx[i - 1]);
                out.indices.push_back(faceIdx[i]);
            }
        }
        else if (type == "mtllib") {
            std::string mtlfile; iss >> mtlfile;
            parseMtlFile(joinPath(baseDir, mtlfile), materials);
        }
        else if (type == "usemtl") {
            std::string name; iss >> name;
            auto it = materials.find(name);
            if (it != materials.end()) {
                const auto kd = it->second.Kd;
                for (auto& v : out.vertices) {
                    v.r = kd.x; v.g = kd.y; v.b = kd.z;
                }

                if (!it->second.map_Kd.empty()) {
                    const std::string texPath = joinPath(baseDir, it->second.map_Kd);
                    auto tex = std::make_shared<Texture>();
                    try {
                        tex->LoadFromFile(WindowDX12::Get().GetGraphicsDevice(), texPath.c_str());
                        out.texture = tex;
                    }
                    catch (...) {
                        std::cerr << "Error texture: " << texPath << "\n";
                        out.texture = defaultWhite;
                    }
                }
                else {
                    out.texture = defaultWhite;
                }
            }
        }
    }

    if (!out.texture)
        out.texture = defaultWhite;
}


std::shared_ptr<MeshAsset> ResourceCache::getMeshFromOBJ(const std::string& path) {
    std::lock_guard<std::mutex> lk(mu_);
    if (auto sp = meshCache_[path].lock())
        return sp;

    auto asset = std::make_shared<MeshAsset>();
    LoadOBJIntoAsset(path, *asset, defaultWhite_);
    asset->Upload(WindowDX12::Get().GetDevice());
    meshCache_[path] = asset;
    return asset;
}