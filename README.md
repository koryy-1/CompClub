# Система учета компьютерного клуба

## Компиляция и запуск:
1. Склонировать репозиторий

```sh
git clone https://github.com/koryy-1/CompClub.git
```

2. Перейти в директироию проекта

```sh
cd CompClub
```

3. Собрать проект

```sh
make
```

4. Запустить скомпилированный файл

```sh
./bin/main input_data/example.txt
```

## Очистка проекта

```sh
make clean
```

## Расчет выручки

Eсли клиент сел за стол, в методе TrackingSystem::Handle() на каждой итерации сначала идет заполнение полей tableId и startTime у ```m_tables[i]->usageSession```, потом если клиент ушел, заполняется поле ```m_tables[i]->usageSession.endTime``` и данные из usageSession заносятся в m_tableUsageSessions.

После обработки всех событий в методе TrackingSystem::CalculateIncome() идет перебор m_tableUsageSessions, вычисляется время использования (usageTime) и выручка (income) для каждого стола.

Это позволяет учитывать случаи, когда клиент, например, пересел с 1 стола на другой.