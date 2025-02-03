# Система учета компьютерного клуба

### Компиляция и запуск
1. Склонировать репозиторий

```sh
git clone https://github.com/koryy-1/CompClub.git
```

2. Перейти в директироию проекта

```sh
cd CompClub.git
```

3. Скомпилировать исходники

```sh
g++ main.cpp fileReader.cpp parser.cpp trackingSystem.cpp console.cpp utils.cpp -o bin/main
```

4. Запустить скомпилированный файл

```sh
./bin/main tests/example.txt
```