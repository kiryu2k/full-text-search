# Конфигурация, сборка и очистка
## Out-of-source build be like:
### Конфигурация
```
cmake -S . -B build/Debug -DCMAKE_BUILD_TYPE=Debug
cmake -S . -B build/Release -DCMAKE_BUILD_TYPE=Release
```
### Сборка
```
cmake --build build/Debug
cmake --build build/Release
```
### Очистка
```
cmake --build build/Debug --target clean
cmake --build build/Release --target clean
rm -rf build
```