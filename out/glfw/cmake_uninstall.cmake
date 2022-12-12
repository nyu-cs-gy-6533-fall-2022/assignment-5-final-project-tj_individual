
if (NOT EXISTS "/Users/tianjiaopei/Documents/0. 阶段专注/硕士学业/2022F/CS6533 CG/Assignment/Final Project/assignment-5-final-project-tj_individual/out/glfw/install_manifest.txt")
    message(FATAL_ERROR "Cannot find install manifest: \"/Users/tianjiaopei/Documents/0. 阶段专注/硕士学业/2022F/CS6533 CG/Assignment/Final Project/assignment-5-final-project-tj_individual/out/glfw/install_manifest.txt\"")
endif()

file(READ "/Users/tianjiaopei/Documents/0. 阶段专注/硕士学业/2022F/CS6533 CG/Assignment/Final Project/assignment-5-final-project-tj_individual/out/glfw/install_manifest.txt" files)
string(REGEX REPLACE "\n" ";" files "${files}")

foreach (file ${files})
  message(STATUS "Uninstalling \"$ENV{DESTDIR}${file}\"")
  if (EXISTS "$ENV{DESTDIR}${file}")
    exec_program("/usr/local/bin/cmake" ARGS "-E remove \"$ENV{DESTDIR}${file}\""
                 OUTPUT_VARIABLE rm_out
                 RETURN_VALUE rm_retval)
    if (NOT "${rm_retval}" STREQUAL 0)
      MESSAGE(FATAL_ERROR "Problem when removing \"$ENV{DESTDIR}${file}\"")
    endif()
  elseif (IS_SYMLINK "$ENV{DESTDIR}${file}")
    EXEC_PROGRAM("/usr/local/bin/cmake" ARGS "-E remove \"$ENV{DESTDIR}${file}\""
                 OUTPUT_VARIABLE rm_out
                 RETURN_VALUE rm_retval)
    if (NOT "${rm_retval}" STREQUAL 0)
      message(FATAL_ERROR "Problem when removing symlink \"$ENV{DESTDIR}${file}\"")
    endif()
  else()
    message(STATUS "File \"$ENV{DESTDIR}${file}\" does not exist.")
  endif()
endforeach()

