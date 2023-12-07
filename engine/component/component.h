//
// Created by Nemo li on 2023/12/6.
//

#ifndef OGL_COMPONENT_H
#define OGL_COMPONENT_H

class GameObject;

class Component {
public:
    Component();

    virtual ~Component();

    GameObject *game_object() {
        return game_object_;
    };

    void set_game_object(GameObject *game_object) {
        game_object_ = game_object;
    };

private:
    GameObject *game_object_;
};


#endif //OGL_COMPONENT_H
