#pragma once

#include <vector>
#include <string>
#include <memory>

#if defined _MSC_VER
// Disable: 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2' ('std::unique_ptr<_Ty>', 'Monitor')
#pragma warning(disable:4251)
#endif

struct __declspec(dllexport) Data {
    double rx;
    double tx;
};

class __declspec(dllexport) Monitor {
public:
    Monitor(size_t = 0); // Default interface_id value of 0 [All Interfaces (*)]

    ~Monitor();

    Data poll();

    std::vector<std::wstring> interfaces();

    void switch_interface(size_t);

private:
    Monitor(const Monitor&);
    Monitor& operator=(const Monitor&);

    class impl;
    std::unique_ptr<impl> pimpl;
};
