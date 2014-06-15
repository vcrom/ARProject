#include "pattern.hpp"
#include <iostream>
using namespace std;

Pattern::Pattern(): oriented(false), patt_width(80.0f),
                    origin(0, 0, 0, 1), xaxis(0, 0, 0, 0), yaxis(0, 0, 0, 0), zaxis(0, 0, 0, 0),
                    averaged_origin(0), averaged_xaxis(0), averaged_yaxis(0), averaged_zaxis(0) {
    patt_center[0] = 0;
    patt_center[1] = 0;
}

void average(const glm::vec4 & candidate, glm::vec4 & average, float & averaged) {
    if (averaged < 1000.0f) {
        average = average + candidate;
        ++averaged;
    }
}

void Pattern::located_origin(const glm::vec4 & candidate) {
    average(candidate, origin, averaged_origin);
}

void Pattern::located_xaxis(const glm::vec4 & candidate) {
    average(candidate, xaxis, averaged_xaxis);
}

void Pattern::located_yaxis(const glm::vec4 & candidate) {
    average(candidate, yaxis, averaged_yaxis);
}

void Pattern::located_zaxis(const glm::vec4 & candidate) {
    average(candidate, zaxis, averaged_zaxis);
}

void Pattern::accomulate_relative_position(ARMarkerInfo *marker_info, const glm::vec4 & position) {
    reference = position;

    double gl_para[16];
    arGetTransMat(marker_info, patt_center, patt_width, patt_trans);
    argConvGlpara(patt_trans, gl_para);

    glm::vec4 origin(0, 0, 0, 1);
    glm::mat4 transform( gl_para[0], gl_para[1], gl_para[2], gl_para[3],
                         gl_para[4], gl_para[5], gl_para[6], gl_para[7],
                         gl_para[8], gl_para[9], gl_para[10], gl_para[11],
                         gl_para[12], gl_para[13], gl_para[14], gl_para[15]);
    glm::vec4 result = transform*origin;
    located_origin(result-position);

    glm::vec4 x(1, 0, 0, 0);
    glm::vec4 result_x = transform*x;
    located_xaxis(result_x);

    glm::vec4 y(0, 1, 0, 0);
    glm::vec4 result_y = transform*y;
    located_yaxis(result_y);

    glm::vec4 z(0, 0, 1, 0);
    glm::vec4 result_z = transform*z;
    located_zaxis(result_z);

    oriented = true;
}


void Pattern::set_position(ARMarkerInfo *marker_info) {
    double gl_para[16];
    arGetTransMat(marker_info, patt_center, patt_width, patt_trans);
    argConvGlpara(patt_trans, gl_para);

  /**  cout << gl_para[0] << " " << gl_para[4] << " " << gl_para[8] << " " << gl_para[12] << endl <<
            gl_para[1] << " " << gl_para[5] << " " << gl_para[9] << " " << gl_para[13] << endl <<
            gl_para[2] << " " << gl_para[6] << " " << gl_para[10]<< " " << gl_para[14]<< endl <<
            gl_para[3]<< " " << gl_para[7]<< " " << gl_para[11]<< " " << gl_para[15] << endl << endl;

**/
    glm::vec4 o(0, 0, 0, 1);
    glm::mat4 transform( gl_para[0], gl_para[1], gl_para[2], gl_para[3],
                         gl_para[4], gl_para[5], gl_para[6], gl_para[7],
                         gl_para[8], gl_para[9], gl_para[10], gl_para[11],
                         gl_para[12], gl_para[13], gl_para[14], gl_para[15]);
    origin = transform*o;

    glm::vec4 x(1, 0, 0, 0);
    xaxis = transform*x;

    glm::vec4 y(0, 1, 0, 0);
    yaxis = transform*y;


    glm::vec4 z(0, 0, 1, 0);
    zaxis = transform*z;

    averaged_origin = averaged_xaxis = averaged_yaxis = averaged_zaxis = 1;
    oriented = true;
}

glm::vec4 Pattern::get_origin() {
    if (oriented) {
        glm::vec3 aux = glm::vec3(origin.x, origin.y, origin.z);
        aux = aux/averaged_origin;

        return glm::vec4(aux.x, aux.y, aux.z, 1.0);
    } else {
        return glm::vec4(0, 0, 0, 1.0);
    }
}

glm::vec4 Pattern::get_global_position()
{
    return reference + get_origin();
}

glm::vec4 Pattern::get_xaxis() {
    if (oriented) {
        glm::vec4 aux = glm::normalize(xaxis);
        return aux;
    } else {
        return glm::vec4(1, 0, 0, 0);
    }
}

glm::vec4 Pattern::get_yaxis() {
    if (oriented) {
        glm::vec4 aux = glm::normalize(yaxis);
        return aux;
    } else {
        return glm::vec4(0, 1, 0, 0);
    }
}

glm::vec4 Pattern::get_zaxis() {
    if (oriented) {
        glm::vec4 aux = glm::normalize(zaxis);
        return aux;
    } else {
        return glm::vec4(0, 0, 1, 0);
    }
}

double* Pattern::get_transformation_matrix() {
    double* matrix = new double[16];
    glm::vec4 o = get_origin();
    matrix[12] = o.x;
    matrix[13] = o.y;
    matrix[14] = o.z;
    matrix[15] = 1.0;

    glm::vec4 x = get_xaxis();
    matrix[0] = x.x;
    matrix[1] = x.y;
    matrix[2] = x.z;
    matrix[3] = 0.0;

    glm::vec4 y = get_yaxis();
    matrix[4] = y.x;
    matrix[5] = y.y;
    matrix[6] = y.z;
    matrix[7] = 0.0;

    glm::vec4 z = get_zaxis();
    matrix[8] = z.x;
    matrix[9] = z.y;
    matrix[10] = z.z;
    matrix[11] = 0.0;

    return matrix;
}
