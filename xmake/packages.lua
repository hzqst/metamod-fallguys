package("bullet3")
    set_homepage("http://bulletphysics.org")
    set_description("Bullet Physics SDK.")
    set_license("zlib")

    set_urls("https://github.com/bulletphysics/bullet3.git")
    add_versions("3.26", "6bb8d1123d8a55d407b19fd3357c724d0f5c9d3c")

    add_patches("3.26", "$(projectdir)/xmake/patches/bullet3.patch", "4a54802943921e9093a9c4de1a1524db25bb1231cfdfad99eab4df07265f75ed")

    add_configs("opencl",           {description = "Build Bullet3OpenCL", default = false, type = "boolean"})
    add_configs("serialize",        {description = "Build Bullet3Serialize", default = false, type = "boolean"})

    add_configs("double_precision", {description = "Enable double precision floats", default = false, type = "boolean"})
    add_configs("extras",           {description = "Build the extras", default = false, type = "boolean"})

    add_configs("OLDER_GLIBC_PATH", {description = "Use older glibc path", default = "", type = "string"})

    add_configs("shared", {description = "Build shared library.", default = false, type = "boolean", readonly = true})
    
    add_deps("cmake")
    add_links("Bullet3Collision", "Bullet3Common", "Bullet3Dynamics", "Bullet3Geometry", "BulletDynamics", "BulletCollision", "BulletInverseDynamics", "BulletSoftBody", "LinearMath")
    add_includedirs("include", "include/bullet")

    if is_plat("mingw") and is_subhost("msys") then
        add_extsources("pacman::bullet")
    elseif is_plat("linux") then
        add_extsources("pacman::bullet", "apt::libbullet-dev")
    elseif is_plat("macosx") then
        add_extsources("brew::bullet")
    end

    on_fetch(function (package)
        if package:config("opencl") then
            package:add("links", "Bullet3OpenCL_clew")
        end
        if package:config("serialize") then
            package:add("links", "Bullet2FileLoader")
        end
    end)

    on_install(function (package)
        local configs = {
            "-DUSE_MSVC_SSE2=FALSE",
            "-DUSE_MSVC_SSE=FALSE",
            "-DUSE_MSVC_AVX=FALSE",
            "-DUSE_MSVC_AVX2=FALSE",
            "-DBUILD_BULLET2_DEMOS=OFF",
            "-DBUILD_CPU_DEMOS=OFF",
            "-DBUILD_OPENGL3_DEMOS=OFF",
            "-DBUILD_UNIT_TESTS=OFF",
            "-DINSTALL_LIBS=ON",
            "-DUSE_GLUT=FALSE",
            "-DUSE_GRAPHICAL_BENCHMARK=FALSE",
            "-DCMAKE_DEBUG_POSTFIX="
        }
        table.insert(configs, "-DBUILD_BULLET3_OPENCL=" .. (package:config("opencl") and "ON" or "OFF"))
        table.insert(configs, "-DBUILD_BULLET3_SERIALIZE=" .. (package:config("serialize") and "ON" or "OFF"))
        table.insert(configs, "-DCMAKE_BUILD_TYPE=" .. (package:debug() and "Debug" or "Release"))
        table.insert(configs, "-DBUILD_SHARED_LIBS=" .. (package:config("shared") and "ON" or "OFF"))
        table.insert(configs, "-DUSE_DOUBLE_PRECISION=" .. (package:config("double_precision") and "ON" or "OFF"))
        table.insert(configs, "-DBUILD_EXTRAS=" .. (package:config("extras") and "ON" or "OFF"))
        table.insert(configs, "-DUSE_MSVC_RUNTIME_LIBRARY_DLL=ON") -- setting this to ON prevents Bullet from replacing flags
        if package:is_plat("windows") and not package:config("vs_runtime"):endswith("d") then
            table.insert(configs, "-DUSE_MSVC_RELEASE_RUNTIME_ALWAYS=ON") -- required to remove _DEBUG from cmake flags
        end
        if package:is_plat("linux") then
            table.insert(configs, "-DBUILD_EGL=OFF")
            table.insert(configs, "-DUSE_SOFT_BODY_MULTI_BODY_DYNAMICS_WORLD=FALSE")
        end
        if is_plat("linux") and package:config("OLDER_GLIBC_PATH") ~= "" then
            table.insert(configs, "-DOLDER_GLIBC_PATH=" .. package:config("OLDER_GLIBC_PATH"))
            table.insert(configs, "-DLINK_AGAINST_OLDER_GLIBC=TRUE")
        end
        import("package.tools.cmake").install(package, configs)
    end)

    on_test(function (package)
        assert(package:check_cxxsnippets({test = [[
            void test(int argc, char** argv) {
                btDefaultCollisionConfiguration collisionConfiguration;
                btCollisionDispatcher dispatcher(&collisionConfiguration);
                btDbvtBroadphase broadphase;
                btSequentialImpulseConstraintSolver constraintSolver;
                btDiscreteDynamicsWorld dynamicWorld(&dispatcher, &broadphase, &constraintSolver, &collisionConfiguration);
                dynamicWorld.setGravity(btVector3(0, -10, 0));

                broadphase.optimize();
            }
        ]]}, {includes = "bullet/btBulletDynamicsCommon.h"}))
    end)
package_end()

package("capstone")
    set_homepage("http://www.capstone-engine.org")
    set_description("Capstone disassembly/disassembler framework for ARM, ARM64 (ARMv8), Alpha, BPF, Ethereum VM, HPPA, LoongArch, M68K, M680X, Mips, MOS65XX, PPC, RISC-V(rv32G/rv64G), SH, Sparc, SystemZ, TMS320C64X, TriCore, Webassembly, XCore and X86.")
    set_license("BSD-3-Clause")

    add_urls("https://github.com/capstone-engine/capstone/archive/refs/tags/$(version).tar.gz",
             "https://github.com/capstone-engine/capstone.git", {submodules = false})

    add_versions("4.0.2", "7c81d798022f81e7507f1a60d6817f63aa76e489aa4e7055255f21a22f5e526a")

    add_patches("4.0.2", "$(projectdir)/xmake/patches/capstone.patch", "35ef8671c7ff15c0036bba2477c2e949229b3006edfab569116fd32eccd0728a")

    add_configs("OLDER_GLIBC_PATH", {description = "Use older glibc path", default = "", type = "string"})

    add_deps("cmake")

    on_install("!iphoneos", function (package)
        local configs = {
            "-DCAPSTONE_ARCHITECHTURE_DEFAULT=FALSE",
            "-DCAPSTONE_X86_SUPPORT=TRUE",
            "-DCAPSTONE_BUILD_CSTOOL=FALSE",
            "-DCAPSTONE_BUILD_TESTS=OFF",
        }
        if is_plat("linux") and package:config("OLDER_GLIBC_PATH") ~= "" then
            table.insert(configs, "-DOLDER_GLIBC_PATH=" .. package:config("OLDER_GLIBC_PATH"))
            table.insert(configs, "-DLINK_AGAINST_OLDER_GLIBC=TRUE")
        end
        table.insert(configs, "-DCAPSTONE_BUILD_SHARED=" .. (package:config("shared") and "ON" or "OFF"))
        table.insert(configs, "-DCAPSTONE_BUILD_STATIC=" .. (package:config("shared") and "OFF" or "ON"))
        table.insert(configs, "-DCMAKE_BUILD_TYPE=" .. (package:is_debug() and "Debug" or "Release"))
        import("package.tools.cmake").install(package, configs)
    end)

    on_test(function (package)
        assert(package:has_cfuncs("cs_version", {includes = "capstone/capstone.h"}))
    end)
package_end()

package("procmap")
    set_homepage("https://github.com/joaomlneto/procmap")
    set_description("C++ wrapper around /proc/self/maps")
    set_license("MIT License")

    add_urls("https://github.com/joaomlneto/procmap.git")

    add_versions("2020.05.14", "e5c329b69a3ee955a1a34355455e56b3ec701271")

    add_patches("2020.05.14", "$(projectdir)/xmake/patches/procmap.patch", "64a0c4ac03580c1b63ddbebcdfcbd449999942d1642e5321262217feb3df73e4")

    add_configs("OLDER_GLIBC_PATH", {description = "Use older glibc path", default = "", type = "string"})

    add_deps("cmake")

    on_install("linux", function (package)
        local configs = {
            "-DCMAKE_BUILD_TYPE=" .. (package:is_debug() and "Debug" or "Release"),
            "-DBUILD_SHARED_LIBS=" .. (package:config("shared") and "ON" or "OFF"),
            "-DBUILD_STATIC_LIBS=" .. (package:config("shared") and "OFF" or "ON")
        }
        if package:config("OLDER_GLIBC_PATH") ~= "" then
            table.insert(configs, "-DOLDER_GLIBC_PATH=" .. package:config("OLDER_GLIBC_PATH"))
            table.insert(configs, "-DLINK_AGAINST_OLDER_GLIBC=TRUE")
        end
        import("package.tools.cmake").install(package, configs)
    end)

    on_test("linux", function (package)
        assert(package:has_cxxtypes("procmap::MemoryMap", {includes = "procmap/MemoryMap.hpp"}))
    end)
package_end()
