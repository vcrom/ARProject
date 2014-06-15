#ifndef PATTERN_HPP
#define PATTERN_HPP

#include <glm/glm.hpp>
#include <AR/gsub.h>
#include <AR/video.h>
#include <AR/param.h>
#include <AR/ar.h>

class Pattern {

public:
    Pattern();

    bool oriented;

    double          patt_width;
    double          patt_center[2];
    double          patt_trans[3][4];

    glm::vec4 reference;

    void located_origin(const glm::vec4 & candidate);
    void located_xaxis(const glm::vec4 & candidate);
    void located_yaxis(const glm::vec4 & candidate);
    void located_zaxis(const glm::vec4 & candidate);

    void accomulate_relative_position(ARMarkerInfo *marker_info, const glm::vec4 & position);
    void set_position(ARMarkerInfo *marker_info);

    glm::vec4 get_origin();
    glm::vec4 get_xaxis();
    glm::vec4 get_yaxis();
    glm::vec4 get_zaxis();
    glm::vec4 get_global_position();

    double* get_transformation_matrix();

private:
    glm::vec4 origin;
    glm::vec4 xaxis;
    glm::vec4 yaxis;
    glm::vec4 zaxis;

    float averaged_origin;
    float averaged_xaxis;
    float averaged_yaxis;
    float averaged_zaxis;
};

#endif // PATTERN_HPP
