#include "QPlaylist.h"
#include "mainwindow.h"
QPlaylist::QPlaylist(QString path, QVector<QMusic*>vec_playlist, QString name_str, QWidget *parent) : QWidget(parent),path_namePlaylist(path), musics_playlist(vec_playlist)
{
    setMaximumWidth(600);
    name = new QLabel();
    name->setText(name_str);
    quantity_mus = new QLabel();
    picture = new QLabel();
    picture->setMinimumSize(60,60);
    picture->setMaximumSize(60,60);
    if(!vec_playlist.empty())
    {
        quantity_mus->setText(QString::number(musics_playlist.size()) + " муз.");
    }
    else quantity_mus->setText("0 муз.");

    mainWidget = new QWidget();
    mainWidget->setMaximumHeight(100);
    QHBoxLayout *mainLayout = new QHBoxLayout();
    QVBoxLayout *layout = new QVBoxLayout();

    musicList_widget = new QWidget();
    list_mus = new QVBoxLayout();
    QHBoxLayout *addSettingLayout= new QHBoxLayout();

    QPushButton *button=new QPushButton("Назад");
    QFont font = button->font();
    font.setBold(true);
    font.setPointSize(14);
    button->setFont(font);
    button->setMinimumHeight(28);
    button->setMaximumWidth(60);
    addSettingLayout->addWidget(button);

    QLabel *name_p=new QLabel(name->text());

    add_setting(addSettingLayout,name_p);
    list_mus->addLayout(addSettingLayout);


    QFont fontLabel = name_p->font();
    fontLabel.setPointSize(10);
    fontLabel.setBold(true);
    name_p->setFont(fontLabel);
    list_mus->addWidget(name_p);

    QVBoxLayout *virtualLayout=new QVBoxLayout();
    for(const auto &it : musics_playlist)
    {
        list_mus->addWidget(it);
    }
    virtualLayout->addLayout(list_mus);
    virtualLayout->addStretch();
    musicList_widget->setLayout(virtualLayout);


    mainLayout->addWidget(picture);
    layout->addWidget(name);
    layout->addWidget(quantity_mus);
    mainLayout->addLayout(layout);

    mainWidget->setLayout(mainLayout);

    QVBoxLayout *lm = new QVBoxLayout(this);
    lm->addWidget(mainWidget); // Добавляем mainWidget в QStackedWidget
    lm->addWidget(musicList_widget);
    musicList_widget->hide();
    mainWidget->setStyleSheet("QWidget:hover { background-color: #505050; }");
    mainWidget->installEventFilter(this);

    connect(button,&QPushButton::clicked,this,&QPlaylist::hide_playlist);
}

void QPlaylist::hide_playlist()
{
    mainWidget->show();
    musicList_widget->hide();
    emit show_(this);
}

void QPlaylist::add_setting(QHBoxLayout *addlayout,QLabel *name_p)
{
    QToolButton *Tools=new QToolButton();
    QIcon icon("img/tools.png");
    Tools->setIcon(icon);

    Tools->setIconSize(QSize(22, 18));
    Tools->setFixedSize(21, 25);

    QMenu *menu = new QMenu(Tools);

    QAction *action1 = new QAction("Змінити назву", Tools);
    QAction *action2 = new QAction("Змінити картинку", Tools);
    QAction *action3 = new QAction("Видалити", Tools);

    menu->addAction(action1);
    menu->addAction(action2);
    menu->addAction(action3);

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
        musicList_widget->hide();
        QWidget *addWidget= new QWidget(this);
        QHBoxLayout *addLayout= new QHBoxLayout(addWidget);
        QPushButton *addbut=new QPushButton("Далі");
        addbut->setMaximumHeight(20);
        QLineEdit *lineEdit=new QLineEdit();
        lineEdit->setMaximumSize(300,25);
        lineEdit->setPlaceholderText("Назва");
        addLayout->addWidget(lineEdit);
        addLayout->addWidget(addbut);
        addWidget->setLayout(addLayout);
        layout()->addWidget(addWidget);
        connect(addbut,&QPushButton::clicked,[=](){
            QString name_=lineEdit->text();
            layout()->removeWidget(addWidget);
            delete addWidget;

            if ( QFile::rename(path_namePlaylist,"playlist/"+name_)) {
                qDebug() << "Название папки успешно изменено!";
            } else {
                qDebug() << "Не удалось изменить название папки";
            }

            name_p->setText(name_);
            name->setText("назва: "+name_);
            musicList_widget->show();

        });
    }); //изменить название плейлиста

    connect(action2, &QAction::triggered, [=](){
        QString selectedFilePath = QFileDialog::getOpenFileName(this, "Выберите файл", "", "All Files (*.*)");

        if (!selectedFilePath.isEmpty()) {
            // Путь, куда нужно скопировать файл
            QString destinationPath = path_namePlaylist+"/";

            // Получаем имя файла из полного пути
            QString fileName = QFileInfo(selectedFilePath).fileName();

            // Создаем объект файла
            QFile sourceFile(selectedFilePath);
            QFile destinationFile(destinationPath + fileName);

            QDir directory(path_namePlaylist);
            QStringList fileList = directory.entryList(QDir::Files);

            for (const QString &fileName : fileList) {
                if (fileName.endsWith(".png", Qt::CaseInsensitive) || fileName.endsWith(".jpg", Qt::CaseInsensitive)) {
                    QString filePath = directory.filePath(fileName);
                    QFile file(filePath);

                    if (file.remove()) {
                        qDebug() << "Файл удален:" << filePath;
                            break; // Выходим из цикла после удаления первой подходящей картинки
                    } else {
                        qDebug() << "Не удалось удалить файл:" << filePath;
                    }
                }
            }  //удаляем прошлую картинку, если она есть
            // Пытаемся скопировать файл
            if (sourceFile.copy(destinationFile.fileName())) {
                qDebug() << "Файл скопирован успешно";
                QPixmap pixmap(path_namePlaylist+"/"+fileName);
                QPixmap scaledPixmap = pixmap.scaled(60, 60, Qt::KeepAspectRatio);
                picture->setPixmap(scaledPixmap);
            } else {
                qDebug() << "Ошибка при копировании файла";
            }
        }

    }); //изменить картинку

    connect(action3, &QAction::triggered, [=](){
        QDir dir(path_namePlaylist);
        if(dir.removeRecursively())
        {
            qDebug()<<"папка удалена";
        }
        else qDebug()<<"ошибка удаления папки";
        hide_playlist();
        delet(this);
    });

    Tools->setStyleSheet("QToolButton { background-color: transparent; border: none;  border-radius: 8px; }"
                  "QToolButton:hover { background-color: #404040; }");

    addlayout->addWidget(Tools);
    addlayout->setAlignment(Tools, Qt::AlignRight);
}

void QPlaylist::change_photo(QPixmap pixmap)
{
    picture->setPixmap(pixmap);
}

void QPlaylist::add_music(QMusic *mus)
{
    ofstream out(path_namePlaylist.toStdString()+"/list.txt",std::ios::app);
    if (out.is_open())
    {
        out << mus->getPath().toStdString() << std::endl;
    }
    else {
        qDebug() << "Ошибка открытия файла";
        return;
    }
    out.close();
    list_mus->addWidget(mus);
    musics_playlist.push_back(mus);
    quantity_mus->setText(QString::number(musics_playlist.size()) + " муз.");
}

void QPlaylist::del_music(QMusic *mus)
{
    QString filePath = path_namePlaylist+"/list.txt";

    // Открываем файл для чтения и записи
    QFile file(filePath);
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        qDebug() << "Ошибка открытия файла";
        return;
    }

    // Читаем содержимое файла в QString
    QTextStream in(&file);
    QString content = in.readAll();
    content.remove(mus->getPath()+"\n");
    // Здесь вы можете произвести необходимые изменения в переменной "content"

    // Закрываем файл
    file.close();

    // Открываем файл заново для записи (перезаписываем содержимое)
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Ошибка открытия файла";
        return;
    }
    // Записываем измененное содержимое обратно в файл
    QTextStream out(&file);
    out << content;

    // Закрываем файл
    file.close();
    QWidget* widgetToDelete;
    for (int i = 0; i < list_mus->count(); ++i) {
        QLayoutItem *item = list_mus->itemAt(i);
        QMusic *qmusicWidget = qobject_cast<QMusic*>(item->widget());

        if (qmusicWidget && qmusicWidget->getPath() == mus->getPath()) {
            widgetToDelete=list_mus->itemAt(i)->widget();
            break;
        }
    }

    list_mus->removeWidget(widgetToDelete);
    list_mus->setGeometry(list_mus->geometry());
    auto it = std::find(musics_playlist.begin(), musics_playlist.end(), widgetToDelete);
    if (it != musics_playlist.end()) {
        // Элемент найден, удаляем его
        musics_playlist.erase(it);
    }
    quantity_mus->setText(QString::number(musics_playlist.size()) + " муз.");
    delete widgetToDelete;
}
