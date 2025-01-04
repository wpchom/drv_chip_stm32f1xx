add_requires("mds_device", "stm32f1xx_device", {
    configs = {
        headeronly = true
    }
})

option("headeronly", function()
    set_default(false)
end)

option("without_interrupt", function()
    set_default(false)
end)

target("drv_chip_stm32f1xx", function()

    add_options("headeronly")
    if get_config("headeronly") then
        set_kind("headeronly")
    else
        set_kind("static")
    end

    add_packages("mds_device", "stm32f1xx_device")

    add_headerfiles("inc/(**.h)")
    add_includedirs("inc")
    add_files("src/*.c")

end)
