if (NOT EXISTS "/home/marina/taller/tpg/Counter2D-2025c1-grupo9/install_manifest.txt")
    message(FATAL_ERROR "Cannot find install manifest: \"/home/marina/taller/tpg/Counter2D-2025c1-grupo9/install_manifest.txt\"")
endif(NOT EXISTS "/home/marina/taller/tpg/Counter2D-2025c1-grupo9/install_manifest.txt")

file(READ "/home/marina/taller/tpg/Counter2D-2025c1-grupo9/install_manifest.txt" files)
string(REGEX REPLACE "\n" ";" files "${files}")
foreach (file ${files})
    message(STATUS "Uninstalling \"$ENV{DESTDIR}${file}\"")
    execute_process(
        COMMAND /home/marina/taller/tpg/Counter2D-2025c1-grupo9/cmake-3.27.9-linux-x86_64/bin/cmake -E remove "$ENV{DESTDIR}${file}"
        OUTPUT_VARIABLE rm_out
        RESULT_VARIABLE rm_retval
    )
    if(NOT ${rm_retval} EQUAL 0)
        message(FATAL_ERROR "Problem when removing \"$ENV{DESTDIR}${file}\"")
    endif (NOT ${rm_retval} EQUAL 0)
endforeach(file)

