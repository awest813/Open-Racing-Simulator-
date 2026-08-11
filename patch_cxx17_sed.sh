sed -i 's/std::optional<std::string>/std::string/' src/libs/client/mainmenu.cpp
sed -i 's/static std::string cachedBackgroundPath;/static std::string cachedBackgroundPath = "";/' src/libs/client/mainmenu.cpp
sed -i 's/if (backgroundPath.has_value()) {/if (!backgroundPath.empty()) {/' src/libs/client/mainmenu.cpp
sed -i 's/GfuiScreenAddBgImg(menuHandle, backgroundPath->c_str());/GfuiScreenAddBgImg(menuHandle, backgroundPath.c_str());/' src/libs/client/mainmenu.cpp
