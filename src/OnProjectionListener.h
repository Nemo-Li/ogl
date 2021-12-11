//
// Created by Nemo li on 2021/12/10.
//

#ifndef OGL_ONPROJECTIONLISTENER_H
#define OGL_ONPROJECTIONLISTENER_H

class OnProjectionListener {
public:
    virtual ~OnProjectionListener() = default;;

    virtual void onPerspectiveChange(float fov, float near, float far, float width, float height);

    virtual void onOrthoChange(float left, float right, float top, float bottom, float near, float far);
};

#endif //OGL_ONPROJECTIONLISTENER_H