#ifndef BOX_H
#define BOX_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <vector>

class Box {
public:
    Box(glm::vec3 center = glm::vec3(0.0f), glm::vec3 extension = glm::vec3(1.0f)) : m_center(center), m_extension(extension) {
        generate();
    }

    Box(float x, float y, float z, float length, float width, float height):
        Box(glm::vec3(x, y, z), glm::vec3(length, width, height)) {}

    ~Box() {
        glDeleteVertexArrays(1, &m_vao);
        glDeleteBuffers(1, &m_vbo);
    }

    void draw() {
        glBindVertexArray(m_vao);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
    }

private:
    glm::vec3 m_center;
    glm::vec3 m_extension;

    GLuint m_vao;
    GLuint m_vbo;
    GLuint m_ebo;

    // Box 局部坐标系说明：
    /*
           Z
           |
           |
           |
           @-----------Y
          /
         /
        /
       X
    */
    std::vector<glm::vec3> getCornerPoints() {
        float halfLength = m_extension.x;
        float halfWidth = m_extension.y;
        float halfHeight = m_extension.z;
        // 生成顶点
        return {
            // 上部四个顶点, 从第一卦限，逆时针
            { halfLength,  halfWidth, halfHeight},
            {-halfLength,  halfWidth, halfHeight},
            {-halfLength, -halfWidth, halfHeight},
            { halfLength, -halfWidth, halfHeight},
            // 下部四个顶点， 从第五卦限，逆时针
            { halfLength,  halfWidth, -halfHeight},
            {-halfLength,  halfWidth, -halfHeight},
            {-halfLength, -halfWidth, -halfHeight},
            { halfLength, -halfWidth, -halfHeight},
        };
    }

    // 正对每个面时的索引顺序为 a b c a c d
    /*
       b----------a
       |         /|
       |       /  |
       |     /    |
       |   /      |
       | /        |
       c----------d
    */
    std::vector<unsigned int> getIndices() {
        return {
            // 前面 px
            0, 3, 7, 0, 7, 4,
            // 右侧面 py
            1, 0, 4, 1, 4, 5,
            // 后面 nx
            2, 1, 5, 2, 5, 6,
            // 左侧面 ny
            3, 2, 6, 3, 6, 7,
            // 顶面 pz
            1, 2, 3, 1, 3, 0,
            // 底面 nz
            4, 7, 6, 4, 6, 5,
        };
    }

    // 正对每个面时的纹理坐标顺序
    /*
       (0,1)-----------(1,1)
       |                   |
       |                   |
       |                   |
       |                   |
       |                   |
       (0,0)-----------(1,0)
    */
    std::vector<glm::vec2> getTextureCoord() {
        return {
            {1.0, 1.0}, {0.0, 1.0}, {0.0, 0.0},
            {1.0, 1.0}, {0.0, 0.0}, {1.0, 0.0}
        };
    }

    std::vector<glm::vec3> getNormal() {
        return {
            { 1, 0, 0}, { 0, 1, 0}, {-1, 0, 0},
            { 0,-1, 0}, { 0, 0, 1}, { 0, 0,-1},
        };
    }
    
    void generate() {
        std::vector<float> vertices = generateVertexData();
        glGenVertexArrays(1, &m_vao);
        glGenBuffers(1, &m_vbo);

        glBindVertexArray(m_vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // texture coord attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        // normal attribute
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);

        // 解绑 VAO
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    std::vector<float> generateVertexData() {
        std::vector<float> vertices;
        auto&& indices = getIndices();
        auto&& cornerPoints = getCornerPoints();
        auto&& textureCoord = getTextureCoord();
        auto&& normal = getNormal();

        int count = 0;
        for (const auto& point : indices) {
            vertices.insert(vertices.end(), {cornerPoints[point].x, cornerPoints[point].y, cornerPoints[point].z});
            vertices.insert(vertices.end(), {normal[count / 6].x, normal[count / 6].y, normal[count / 6].z});
            vertices.insert(vertices.end(), {textureCoord[count % 6].x, textureCoord[count % 6].y});
            count++;
        }
        return vertices;
    }
};

#endif // BOX_H
