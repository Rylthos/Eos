#pragma once

#include "Eos/Core/Application.hpp"

extern Eos::Application* Eos::createApplication();

int main(int argc, char** argv)
{
    auto app = Eos::createApplication();

    app->start();

    delete app;
}