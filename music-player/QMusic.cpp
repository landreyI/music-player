#include "QMusic.h"
#include "mainwindow.h"

QMusic::QMusic(QString path1,QWidget *parent) : QWidget(parent),path(path1)
{
    setMaximumWidth(600);
    setMaximumHeight(60);
    name=new QLabel("???");
    artist=new QLabel("???");

    QWidget *mainW=new QWidget(this);

    QVBoxLayout  *vlayout = new QVBoxLayout();
    vlayout->setSpacing(0);
    vlayout->addWidget(name);
    vlayout->addWidget(artist);
    vlayout->addStretch();

    QHBoxLayout  *hlayout = new QHBoxLayout ();
    hlayout->addLayout(vlayout);
    pushTool(hlayout);

    hlayout->setContentsMargins(10, 6, 10, 6);
    mainW->setLayout(hlayout);
    QVBoxLayout *gf=new QVBoxLayout(this);
    gf->addWidget(mainW);
    setStyleSheet("QWidget:hover { background-color: #505050; }");



    installEventFilter(this);
}

QMusic::~QMusic()
{
    delete name;
    delete artist;
}

QMusic::QMusic(const QMusic& other):QWidget(other.parentWidget()), path(other.path) {
    name = new QLabel(other.name->text());
    artist = new QLabel(other.artist->text());
    album = other.album;
    year = other.year;
    genre = other.genre;

    setMaximumWidth(600);
    setMaximumHeight(60);

    QWidget *mainW=new QWidget(this);

    QVBoxLayout  *vlayout = new QVBoxLayout();
    vlayout->setSpacing(0);
    vlayout->addWidget(name);
    vlayout->addWidget(artist);
    vlayout->addStretch();

    QHBoxLayout  *hlayout = new QHBoxLayout ();
    hlayout->addLayout(vlayout);
    pushTool(hlayout);

    hlayout->setContentsMargins(10, 6, 10, 6);
    mainW->setLayout(hlayout);
    QVBoxLayout *gf=new QVBoxLayout(this);
    gf->addWidget(mainW);
    setStyleSheet("QWidget:hover { background-color: #505050; }");

    installEventFilter(this);
}

void QMusic::pushTool(QHBoxLayout *hlayout)
{
    QToolButton *Tools=new QToolButton();
    QIcon icon("img/tools.png");
    Tools->setIcon(icon);

    Tools->setIconSize(QSize(22, 18));
    Tools->setFixedSize(21, 25);

    QMenu *menu = new QMenu(Tools);

    QAction *action1 = new QAction("Увімкнути наступною", Tools);
    QAction *action2 = new QAction("Добавить в плейлист", Tools);
    QAction *action3 = new QAction("Повна інформація", Tools);
    QAction *action4 = new QAction("Видалити", Tools);

    menu->addAction(action1);
    menu->addAction(action2);
    menu->addAction(action3);
    menu->addAction(action4);

    connect(Tools, &QToolButton::clicked, [=](){
        if (menu->isVisible()) {
            menu->hide();
        } else {
            // Получение геометрии кнопки
            QRect buttonGeometry = Tools->geometry();
            // Расчет позиции для меню
            QPoint menuPos = mapToGlobal(QPoint(buttonGeometry.right(), buttonGeometry.bottom()));
            // Отображение меню
            menu->exec(menuPos);
        }
    });

    connect(action1, &QAction::triggered, [=](){
        path_next = path;
    });
    connect(action2, &QAction::triggered, [=](){
        emit add_playlist(this);
    });
    connect(action3, &QAction::triggered, [=](){
        emit inf_mus(this);
    });
    connect(action4, &QAction::triggered, [=](){
        emit delet(this);
    });

    Tools->setStyleSheet("QToolButton { background-color: transparent; border: none;  border-radius: 8px; }"
                  "QToolButton:hover { background-color: #404040; }");

    hlayout->addWidget(Tools);
    hlayout->setAlignment(Tools, Qt::AlignTop);
}

QMusic& QMusic::operator=(const QMusic& other) {
    if (this == &other) { // Проверка на самоприсваивание
        return *this;
    }
    // Копирование данных из other в текущий объект
    path = other.path;
    name = new QLabel(other.name->text());
    artist = new QLabel(other.artist->text());
    album = other.album;
    year = other.year;
    genre = other.genre;
    return *this;
}
