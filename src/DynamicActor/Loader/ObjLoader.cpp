#include "ObjLoader.hpp"

#include <iostream>

#include "fs.h"

#include "logger/Logger.hpp"

#include <sead/heap/seadHeapMgr.h>
#include <sead/heap/seadFrameHeap.h>
#include <sead/heap/seadExpHeap.h>

template <typename T>
struct Buffer {
    T* buffer;
    size_t length;

    size_t getSizeInBytes() const {
        return length * sizeof(T);
    }
};

namespace al {
    sead::Heap *getCurrentHeap(void);
    sead::Heap* getStationedHeap();
    sead::Heap* getSequenceHeap();
    sead::Heap* getSceneHeap();
    sead::Heap* getWorldResourceHeap();
};

class membuf : public std::basic_streambuf<char> {
// https://tuttlem.github.io/2014/08/18/getting-istream-to-work-off-a-byte-array.html
public:
  membuf(const uint8_t *p, size_t l) {
    setg((char*)p, (char*)p, (char*)p + l);
  }
};

class memstream : public std::istream {
// https://tuttlem.github.io/2014/08/18/getting-istream-to-work-off-a-byte-array.html
public:
  memstream(const uint8_t *p, size_t l) :
    std::istream(&_buffer),
    _buffer(p, l) {
    rdbuf(&_buffer);
  }

private:
  membuf _buffer;
};


Buffer<u8> loadFile(const char* file_path, sead::Heap* heap) {
    Logger::log("loadFile start\n");

    nn::fs::FileHandle file_handle{};
    auto r = nn::fs::OpenFile(&file_handle, file_path, nn::fs::OpenMode_Read);
    if (R_FAILED(r)) {
        Logger::log("openfile failed\n");
        return {};
    }

    Logger::log("openfile success\n");

    long fileSize;
    r = nn::fs::GetFileSize(&fileSize, file_handle);
    if (R_FAILED(r)) {
        Logger::log("getfilesize failed\n");
        return {};
    }
    Logger::log("got file stats: size %ld bytes\n", fileSize);

    Logger::log("about to allocate file buffer: %ld bytes\n", fileSize);
    Buffer<u8> buffer{(u8*)heap->tryAlloc(fileSize, 1), 0};

    Logger::log("allocated buf\n");

    ulong bytesRead = 0;
    r = nn::fs::ReadFile(&bytesRead, file_handle, 0, buffer.buffer, fileSize);
    if (R_FAILED(r)) {
        Logger::log("readfile failed\n");
        return {};
    };
    buffer.length = bytesRead;
    Logger::log("read file: %ld bytes\n", bytesRead);

    nn::fs::CloseFile(file_handle);

    return buffer;
}

namespace dynamicactor::loader {

bool ObjLoader::loadMesh(const char* objFilename, al::DynamicDrawActor* dynamicDrawActor, al::DynamicCollisionActor* dynamicCollisionActor, int dynamicCollisionAttributeIndex) {
    if (!dynamicDrawActor) {
        return false;
    }

    Logger::log("Start loadMesh\n");

    sead::Heap* heap = sead::FrameHeap::create(0x1000000, "DynamicMeshHeap", al::getWorldResourceHeap(), 8, sead::Heap::cHeapDirection_Forward, false);
    Logger::log("Created Heap: %ld\n", heap);

    auto objFileBuffer = loadFile(objFilename, heap);
    if (!objFileBuffer.buffer) {
        heap->destroy();
        return false;
    }

    Logger::log("loadFile complete\n");

    memstream inStream(objFileBuffer.buffer, objFileBuffer.getSizeInBytes());

    Logger::log("inStream complete\n");

    std::string line;
    size_t vertexCount = 0;
    size_t normalCount = 0;
    size_t texCoordCount = 0;
    size_t triCount = 0;
    while (std::getline(inStream, line))
    {
        if (line.rfind("v ", 0) == 0) {
            vertexCount++;
        }
        else if (line.rfind("vn ", 0) == 0) {
            normalCount++;
        }
        else if (line.rfind("vt ", 0) == 0) {
            texCoordCount++;
        }
        else if (line.rfind("f ", 0) == 0) {
            triCount++;
        }
    }

    Logger::log("Initial file pass complete, verts: %ld, tris: %ld\n", vertexCount, triCount);
    Logger::log("Max allocatable size: %.4fkb\n", heap->getMaxAllocatableSize(8)/1024.f);
    Logger::log("About to allocate buffers: %.0fkb, %.0fkb\n", (sizeof(sead::Vector3f)*vertexCount)/1024.f, (sizeof(sead::Vector3f)*normalCount)/1024.f);

    Buffer<sead::Vector3f> VERTEX_BUFFER = Buffer<sead::Vector3f>{(sead::Vector3f*)heap->tryAlloc(sizeof(sead::Vector3f)*vertexCount, 16), vertexCount};
    Buffer<sead::Vector3f> NORMAL_BUFFER = Buffer<sead::Vector3f>{(sead::Vector3f*)heap->tryAlloc(sizeof(sead::Vector3f)*normalCount, 16), normalCount};
    Buffer<sead::Vector2f> TEXCOORD_BUFFER = Buffer<sead::Vector2f>{(sead::Vector2f*)heap->tryAlloc(sizeof(sead::Vector2f)*texCoordCount, 16), texCoordCount};

    Logger::log("Allocated buffers: %.0fkb, %.0fkb\n", (sizeof(sead::Vector3f)*vertexCount)/1024.f, (sizeof(sead::Vector3f)*normalCount)/1024.f);

    // Reset memstream
    inStream = memstream(objFileBuffer.buffer, objFileBuffer.getSizeInBytes());

    size_t vertexIndex = 0;
    size_t normalIndex = 0;
    size_t texCoordIndex = 0;
    dynamicDrawActor->begin();
    if (dynamicCollisionAttributeIndex != -1) {
        dynamicCollisionActor->begin();
        dynamicCollisionActor->attribute(dynamicCollisionAttributeIndex);
    }
    while (std::getline(inStream, line))
    {
        auto skipToNextValue = [&line]() {
            while (!line.empty() && line[0] != ' ') {
                line = line.substr(1);
            }
            if (!line.empty()) {
                line = line.substr(1); // skip " "
            }
        };
        if (line.rfind("v ", 0) == 0) {
            sead::Vector3f& v = VERTEX_BUFFER.buffer[vertexIndex];
            line = line.substr(2); // skip "v "
            v.x = std::stof(line);
            skipToNextValue();
            v.y = std::stof(line);
            skipToNextValue();
            v.z = std::stof(line);
            skipToNextValue();
            vertexIndex++;
        }
        else if (line.rfind("vn ", 0) == 0) {
            sead::Vector3f& n = NORMAL_BUFFER.buffer[normalIndex];
            line = line.substr(3); // skip "vt "
            n.x = std::stof(line);
            skipToNextValue();
            n.y = std::stof(line);
            skipToNextValue();
            n.z = std::stof(line);
            skipToNextValue();
            normalIndex++;
        }
        else if (line.rfind("vt ", 0) == 0) {
            sead::Vector2f& tx = TEXCOORD_BUFFER.buffer[texCoordIndex];
            line = line.substr(3); // skip "vt "
            tx.x = std::stof(line);
            skipToNextValue();
            tx.y = std::stof(line);
            skipToNextValue();
            texCoordIndex++;
        }
        else if (line.rfind("f ", 0) == 0) {
            line = line.substr(2); // skip "f "
            for (int i = 0; i < 3; i++) {
                size_t pos = 0;
                auto vertexIndex = std::stoi(line, &pos) - 1; // -1 since wavefront obj indexing starts of 1
                line = line.substr(pos + 1 /* +1 for the '/' */);
                auto texCoordIndex = std::stoi(line, &pos) - 1;
                line = line.substr(pos + 1 /* +1 for the '/' */);
                auto normalIndex = std::stoi(line, &pos) - 1;
                skipToNextValue();

                sead::Vector3f basePos(0,0,0);//-5750.0,1645.0,2539.7);
                auto transform = [&](sead::Vector3f const& p) { 
                    float scale = 200.f;
                    sead::Vector3f p1 = basePos + scale*p;
                    // p1.y += 50.f*cosf(p1.x/100.f + 3*timeOffset);
                    return p1;
                };

                dynamicDrawActor->normal(NORMAL_BUFFER.buffer[normalIndex]);
                dynamicDrawActor->tangent(sead::Vector4f(1,0,0,1));
                auto const& tx = TEXCOORD_BUFFER.buffer[texCoordIndex];
                dynamicDrawActor->texCoord(sead::Vector2f(tx.x, 1.f-tx.y), 0);
                dynamicDrawActor->texCoord(sead::Vector2f(tx.x, 1.f-tx.y), 1);
                dynamicDrawActor->vertex(transform(VERTEX_BUFFER.buffer[vertexIndex]));

                if (dynamicCollisionAttributeIndex != -1) {
                    dynamicCollisionActor->vertex(transform(VERTEX_BUFFER.buffer[vertexIndex]));
                }
            }
        }
    }
    dynamicDrawActor->end();
    if (dynamicCollisionAttributeIndex != -1) {
        dynamicCollisionActor->end();
    }

    Logger::log("Finished second pass to parse mesh\n");

    heap->destroy();
    Logger::log("Free-d sub heap\n");

    return true;
}

}