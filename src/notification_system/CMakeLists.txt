#initializer's dependencies
set(DEPENDENCIES server)
set(LIBRARY_NAME notification_system)
#creating library
add_library(${LIBRARY_NAME}
    queue_notifier.hpp
    queue_notifier.cpp
    session_state_notifier.cpp
    session_state_notifier.hpp
)

#including and linking dependencies
target_link_libraries(${LIBRARY_NAME} PUBLIC ${DEPENDENCIES})
foreach (dir IN ITEMS ${DEPENDENCIES})
    target_include_directories(${LIBRARY_NAME} PUBLIC "../${dir}")
endforeach ()