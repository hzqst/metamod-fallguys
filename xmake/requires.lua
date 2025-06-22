local older_glibc_path = path.join(os.projectdir(), "thirdparty/glibc_224/force_link_glibc_2.24.h")

add_requires("capstone 4.0.2", {configs = {shared = false, OLDER_GLIBC_PATH = older_glibc_path}})
add_requires("bullet3", {configs = {shared = false, OLDER_GLIBC_PATH = older_glibc_path}})
if is_plat("!windows") then
    add_requires("procmap 2020.05.14", {configs = {OLDER_GLIBC_PATH = older_glibc_path}})
end

add_requires("libcurl", "openssl")
