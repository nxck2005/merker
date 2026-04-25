add_rules("mode.debug", "mode.release")
add_requires("glm")
target("merker")
    add_packages("glm")
    set_kind("binary")
    set_languages("c++20")
    add_files("src/*.cpp")

    -- Vulkan
    add_includedirs("C:/VulkanSDK/1.4.341.1/include") -- please change these includes later
    add_linkdirs("C:/VulkanSDK/1.4.341.1/Lib")
    add_links("vulkan-1")

    -- GLFW and GLM via vcpkg
    add_includedirs("C:/vcpkg/installed/x64-windows/include")
    add_linkdirs("C:/vcpkg/installed/x64-windows/lib")
    add_links("glfw3dll")
