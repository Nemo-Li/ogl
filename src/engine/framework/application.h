//
// Created by Nemo li on 2023/12/6.
//

#ifndef OGL_APPLICATION_H
#define OGL_APPLICATION_H

#include <string>

class Application {
public:
    static const std::string &data_path() { return data_path_; }

    static void set_data_path(std::string data_path) { data_path_ = data_path; }

private:
    static std::string data_path_;
};


#endif //OGL_APPLICATION_H
