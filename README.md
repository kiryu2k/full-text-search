# Конфигурация, сборка и очистка
## Out-of-source build be like:
### Конфигурация
```
cmake --preset debug
cmake --preset release
```
### Сборка
```
cmake --build --preset debug
cmake --build --preset release
```
### Очистка
```
cmake --build build/debug --target clean
cmake --build build/release --target clean
rm -rf build
```