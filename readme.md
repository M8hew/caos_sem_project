# Файловый менеджер

## Сборка
    Программа собирается стандартным образом
    '''mkdir build && cd build
    cmake .. && make
    ./имя_вашей_программы''' 

## Реализованый функционал
    ** Закрытие производиться нажатием кнопки 'q'. **

    ** Подсветка названий **
    Названия директорий подсвечены синим, текстовые ссылки красным, а всё остальное белым
    
    ** Переход по директориям **
    Переход по диреекториям осуществляется с помощи клавиши 'enter', если в директорию перейти нельзя, то мы на остёмся в той же директории
    
    ** Удаление **
    Удаление файлов и директорий происходит тогда когда на это есть права, если прав нет, то директория/файл просто остаются как были. При этом директории можно удалять только если они пустые.

    ** Окно терминала произвольного размера **
    Вывод адаптирован для окон разного размера, т.к. для вывода строк используется оберезание по длине буфера, который расчитывается исходя из размеров окна терминала.

    ** Дополнительная подсветка **
    Символьные ссылки и FIFO отмечены красным и залёным соотвественно
