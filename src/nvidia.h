#pragma once
#ifdef _WIN32
extern "C" {
    _declspec(dllexport) unsigned long int NvOptimusEnablement = 0x00000001;
}
#endif