//
// Created by Nemo li on 2023/12/6.
//

#ifndef OGL_APPLICATION_H
#define OGL_APPLICATION_H

#include <string>

class ApplicationBase;

class Application {
public:
    static void Init(ApplicationBase *instance);

    static const std::string &data_path();

    static void Run();

private:
    static ApplicationBase *instance_;
};


#endif //OGL_APPLICATION_H
