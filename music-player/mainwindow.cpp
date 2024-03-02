#include "mainwindow.h"
#include "read_folder.h"
#include <iostream>
#include <fstream>
#include <string>

QString readStringFromStream(QDataStream& stream, int length);
MetadataText readMP3Metadata(const QString& filename, MetadataText& metadata);

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    readFolder("mus");
    resize(450, 600);
    player = new QMediaPlayer;
    mainWidget = new QWidget(this);
    setCentralWidget(mainWidget);

    mainlayout = new QVBoxLayout(mainWidget);

    addSettingBut();

    read_playlist("playlist");
    mainlayout->addWidget(playlistwidget);

    playlistwidget->hide();
    {
        list_of_music=new QWidget(mainWidget);
        QVBoxLayout *list_of_music_layout=new QVBoxLayout(list_of_music);
        for(auto it:musics_vec)
        {
            list_of_music_layout->addWidget(it);
            connect(it, &QMusic::path_, this, [=](QMusic *mus) {
                if(player->mediaStatus() == QMediaPlayer::PlayingState)
                {
                   QUrl Url=player->source();
                   QString path_play=Url.toLocalFile();
                   if(mus->getPath()==path_play)
                   {
                       qint64 currentPosition=player->position();
                       emit disconnect_play();
                       play_mus(mus,list_of_music,musics_vec);
                       player->setPosition(currentPosition);
                   }
                }
                else
                {
                    play_mus(mus,list_of_music,musics_vec);
                }
            }); //создание панели для работы

            connect(it, &QMusic::delet, this, &MainWindow::delet_mus); //удаление музыки файл

            connect(it,&QMusic::add_playlist,[=](QMusic *mus){
                list_of_music->hide();
                QWidget *addChoose= new QWidget();
                addChoose->setMaximumWidth(500);
                QVBoxLayout *addChooseLayout= new QVBoxLayout();
                QHBoxLayout *exitL=new QHBoxLayout();
                QLabel *label=new QLabel("Оберіть плейлист");
                exitL->addWidget(label);
                QPushButton *exit=new QPushButton("Відмінити");
                exit->setMaximumWidth(90);
                exit->setMinimumHeight(30);
                exitL->addWidget(exit);
                addChooseLayout->addLayout(exitL);
                QVBoxLayout *virtualLayout= new QVBoxLayout(addChoose);
                virtualLayout->addLayout(addChooseLayout);
                virtualLayout->addStretch();
                for (int i = 0; i < playlistlayout->count(); ++i) {
                    QLayoutItem *item = playlistlayout->itemAt(i);
                    QWidget *widget = item->widget();

                    QPlaylist *playlistWidget = qobject_cast<QPlaylist*>(widget);
                    if (playlistWidget) {
                        QPushButton *chooseBut = new QPushButton(playlistWidget->getName());

                        QFont font = chooseBut->font();
                        font.setPointSize(10);
                        font.setBold(true);
                        chooseBut->setFont(font);

                        chooseBut->setMinimumHeight(30);
                        chooseBut->setMaximumWidth(300);
                        addChooseLayout->addWidget(chooseBut);
                        addChooseLayout->addStretch();
                        addChoose->setLayout(virtualLayout);
                        mainlayout->addWidget(addChoose);
                        connect(exit, &QPushButton::clicked, [=](){
                            mainlayout->removeWidget(addChoose);
                            delete addChoose;
                            list_of_music->show();
                            return;
                        });

                        connect(chooseBut, &QPushButton::clicked, [=]() {
                            mainlayout->removeWidget(addChoose);
                            delete addChoose;
                            QVector<QMusic*> vec=playlistWidget->getVec();
                            for(auto &it:vec)
                            {
                                if(mus->getPath()==it->getPath()){
                                    list_of_music->show();
                                    return;
                                }
                            }
                            QMusic *copy=new QMusic(mus->getPath());
                            copy->setName(mus->getName());
                            playlistWidget->add_music(copy);
                            list_of_music->show();
                        });
                    }
                }
            }); //добавить в плейлист

            connect(it,&QMusic::inf_mus,[=](QMusic *mus){
                QWidget *inf_mus= new QWidget();
                QVBoxLayout *layout= new QVBoxLayout();
                QPushButton *exit= new QPushButton("Назад");
                exit->setMaximumWidth(45);
                exit->setMinimumHeight(25);
                QLabel *inf_label= new QLabel();
                QLabel *inf_path= new QLabel();
                inf_label->setMaximumSize(600,190);
                inf_label->setStyleSheet("font-size: 10pt;"
                                         "background-color: #505050; border-radius: 15px;");
                QString inf_str = "<br><span style=\"color:#909090; font-size: 16px;\">Композиція</span><br>" +
                 mus->getName() + "<br><br>" +
                                  "<span style=\"color:#909090; font-size: 16px;\">Виконавець</span><br>" +
                 mus->getArtist() + "<br><br>" +
                                  "<span style=\"color:#909090; font-size: 16px;\">Альбом</span><br>" +
                 mus->getAlbum() + "<br><br>";
                inf_label->setText(inf_str);
                QFont font = inf_label->font();
                font.setBold(true); // Устанавливаем жирный стиль
                inf_label->setFont(font);
                inf_label->setAlignment(Qt::AlignCenter);
                exit->setFont(font);

                QString inf_path_str= "Жанр - "+mus->getGenre()+"\n"+
                                      "Рік - "+QString::number(mus->getYear())+"\n"+
                                      "Місце знаходження - "+mus->getPath()+"\n";
                inf_path->setText(inf_path_str);
                inf_path->setFont(font);
                inf_path->setStyleSheet("color: #909090;");
                layout->addWidget(exit);
                layout->addWidget(inf_label);
                layout->addWidget(inf_path);
                layout->addStretch();

                inf_mus->setLayout(layout);
                if(change) playlistwidget->hide();
                else list_of_music->hide();

                playback_settings_widget->hide();
                mainlayout->addWidget(inf_mus);
                connect(exit,&QPushButton::clicked,[=](){
                    mainlayout->removeWidget(inf_mus);
                    delete inf_mus;
                    if(change) playlistwidget->show();
                    else list_of_music->show();
                    QUrl Url=player->source();
                    QString path=Url.toLocalFile();
                    if(path!="")
                    playback_settings_widget->show();
                });
            }); //полная информация о музыке
        }
        list_of_music_layout->addStretch();
        list_of_music->setLayout(list_of_music_layout);
        mainlayout->addWidget(list_of_music);
        reproduce();
    }
    mainWidget->setLayout(mainlayout);
    setStyleSheet("QPushButton { background-color: transparent; border: none;  border-radius: 8px; }"
                  "QPushButton:hover { background-color: #505050; }");
}

void MainWindow::reproduce()
{
        playStopButton = new QPushButton();

        nameLabel=new QLabel();
        nameLabel->setStyleSheet("font-size: 16px;");

        off=new QPushButton("X");
        off->setStyleSheet("font-size: 20px;");
        off->setMaximumWidth(30);

        hide =new QPushButton();

        hide->setCheckable(true);  // Делаем кнопку переключаемой (toggleable)
        hide->setChecked(false);

        previous =new QPushButton();
        previous->setIcon(QIcon("img/left.png"));
        previous->setMaximumSize(30,40);

        next =new QPushButton();
        next->setIcon(QIcon("img/right.png"));
        next->setMaximumSize(30,40);

        slider = new QSlider(Qt::Horizontal);
        slider->setStyleSheet("QSlider::groove:horizontal { border: 1px solid #999999; height: 3px; margin: 2px 0; } "
                              "QSlider::handle:horizontal { background: #888888; border: 1px solid #888888; width: 5px; height: 5px; border-radius: 6px; margin: -2px 0; }");

        //waveformWidget= new AudioWaveformWidget();
        QGridLayout *playback_settings=new QGridLayout();


        playback_settings->addWidget(hide, 0, 0, 1, 2, Qt::AlignHCenter); // Первый ряд, первая и вторая колонка
        playback_settings->addWidget(off, 0, 3);
        playback_settings->addWidget(nameLabel, 1, 0, 1, 2); // Второй ряд, первая и вторая колонка

        // Третий ряд, четвертая колонка
        //playback_settings->addWidget(waveformWidget,2,1);
        playback_settings->addWidget(slider, 3, 1); // Четвёртый ряд, вторая колонка

        playback_settings->addWidget(previous, 4, 0);
        playback_settings->addWidget(playStopButton, 4, 1,  Qt::AlignHCenter); // Четвёртый ряд, первая колонка
        playback_settings->addWidget(next, 4, 3, Qt::AlignRight);

        playback_settings->setRowStretch(0, 1); // Установка пропорции для первого ряда (больше места)
        playback_settings->setRowStretch(1, 0); // Установка пропорции для второго ряда (больше места)
        playback_settings->setRowStretch(2, 3); // Установка пропорции для третьего ряда (меньше места)
        playback_settings->setRowStretch(3, 1); // Установка пропорции для четвёртого ряда (меньше места)

        playback_settings->setColumnStretch(0, 1); // Установка пропорции для первой колонки
        playback_settings->setColumnStretch(1, 1); // Установка пропорции для второй колонки
        playback_settings->setColumnStretch(3, 0);

        playback_settings_widget=new QWidget();
        playback_settings_widget->setLayout(playback_settings);
        mainlayout->addWidget(playback_settings_widget);

        playback_settings_widget->hide();
}

void MainWindow::play_mus(QMusic *mus, QWidget *hide_widget, QVector<QMusic*> musics_vec)
{
    QIcon icon_pause("img/pause.png");
    QIcon icon_play("img/play.png");
    QIcon icon_hide("img/hide.png");
    QIcon icon_open("img/open.png");

    if(player->mediaStatus() == QMediaPlayer::NoMedia)
    {
        hide_widget->hide();
        playback_settings_widget->show();
        nameLabel->setText(mus->getName());

        playStopButton->setIcon(icon_pause);
        playStopButton->setIconSize(QSize(25, 15));
        playStopButton->setMinimumHeight(24);
        hide->setIcon(icon_hide);
        hide->setIconSize(QSize(22, 22));

        QTimer *sliderUpdateTimer = new QTimer();
        sliderUpdateTimer->start(100);

        QAudioOutput *audioOutput = new QAudioOutput;
        player->setAudioOutput(audioOutput);
        player->setSource(QUrl::fromLocalFile(mus->getPath()));

        player->play();
        //waveformWidget->loadMp3File("mus/zvuk-peniya-32541.mp3");
        isPlaying=true;

        connect(previous, &QPushButton::clicked, this,[=]() {
            if(path_next!="")
            {
                player->setSource(QUrl::fromLocalFile(path_next));
                player->play();
                auto it = std::find_if(musics_vec.begin(),musics_vec.end(),[](QMusic *a)
                                       {
                                           return a->getPath()==path_next;
                                       });
                int index = std::distance(musics_vec.begin(), it);
                nameLabel->setText(musics_vec[index]->getName());
                path_next="";
            }
            else
            {
                QUrl Url=player->source();
                QString path=Url.toLocalFile();
                auto it = std::find_if(musics_vec.begin(),musics_vec.end(),[=](QMusic *a)
                                       {
                                           return a->getPath()==path;
                                       });
                if (it != musics_vec.begin()) {
                    int index = std::distance(musics_vec.begin(), it);
                    player->setSource(QUrl::fromLocalFile(musics_vec[index-1]->getPath()));
                    nameLabel->setText(musics_vec[index-1]->getName());
                    player->play();
                } else {
                    player->setSource(QUrl::fromLocalFile(musics_vec.back()->getPath()));
                    nameLabel->setText(musics_vec.back()->getName());
                    player->play();
                }
            }
        });  //предыдущий трек

        connect(next, &QPushButton::clicked, this,[=]() {
            if(path_next!="")
            {
                player->setSource(QUrl::fromLocalFile(path_next));
                player->play();
                auto it = std::find_if(musics_vec.begin(),musics_vec.end(),[=](QMusic *a)
                                       {
                                           return a->getPath()==path_next;
                                       });
                int index = std::distance(musics_vec.begin(), it);
                nameLabel->setText(musics_vec[index]->getName());
                path_next="";
            }
            else
            {
                QUrl Url=player->source();
                QString path=Url.toLocalFile();
                auto it = std::find_if(musics_vec.begin(),musics_vec.end(),[=](QMusic *a)
                                       {
                                           return a->getPath()==path;
                                       });
                if (it+1 !=  musics_vec.end()) {
                    int index = std::distance(musics_vec.begin(), it);
                    player->setSource(QUrl::fromLocalFile(musics_vec[index+1]->getPath()));
                    nameLabel->setText(musics_vec[index+1]->getName());
                    player->play();
                } else {
                    player->setSource(QUrl::fromLocalFile(musics_vec.first()->getPath()));
                    nameLabel->setText(musics_vec.first()->getName());
                    player->play();
                }
            }
        });  //следующий трек

        connect(sliderUpdateTimer, &QTimer::timeout, this, [=](){
            updateSliderPosition(slider);
        }); //ползунок за музыкой

        connect(slider, &QSlider::sliderMoved, this,[=](int position){
            setPlaybackPosition(position,slider);
        }); //перетаскивание ползунка

        connect(hide, &QPushButton::clicked,this,[=]() {
            if (hide->isChecked()) {
                if(hide_widget==playlistwidget)
                {showPlaylist();}
                if(hide_widget==list_of_music)
                {showMusList();}
                hide->setIcon(icon_open);
            } else {
                if(hide_widget==playlistwidget)
                {hidePlaylist();}
                if(hide_widget==list_of_music)
                {hideMusList();}
                hide->setIcon(icon_hide);
            }
        });  //скрыть

        connect(playStopButton, &QPushButton::clicked, this,[=]() {
            if (isPlaying) {
                player->pause();
                playStopButton->setIcon(icon_play);
            } else {
                player->play();
                playStopButton->setIcon(icon_pause);
            }
            isPlaying = !isPlaying;
        }); //запустить или остановить

        connect(off, &QPushButton::clicked,this, [=]() {
            player->stop();
            player->setSource(QUrl());

            disconnect(sliderUpdateTimer, &QTimer::timeout, this, nullptr);
            disconnect(hide, &QPushButton::clicked, this, nullptr);
            disconnect(previous, &QPushButton::clicked, this, nullptr);
            disconnect(next, &QPushButton::clicked, this, nullptr);
            disconnect(slider, &QSlider::sliderMoved, this, nullptr);
            disconnect(playStopButton, &QPushButton::clicked, this, nullptr);
            disconnect(player, &QMediaPlayer::mediaStatusChanged, this, nullptr);
            playback_settings_widget->hide();
            if(hide_widget==playlistwidget)
            {showPlaylist();}
            if(hide_widget==list_of_music)
            {showMusList();}
            isPlaying=false;
            disconnect(off, &QPushButton::clicked, this, nullptr);
            return;
        }); //выключить | удалить

        connect(this,&MainWindow::disconnect_play,[=](){
            player->stop();
            player->setSource(QUrl());

            disconnect(sliderUpdateTimer, &QTimer::timeout, this, nullptr);
            disconnect(hide, &QPushButton::clicked, this, nullptr);
            disconnect(previous, &QPushButton::clicked, this, nullptr);
            disconnect(next, &QPushButton::clicked, this, nullptr);
            disconnect(slider, &QSlider::sliderMoved, this, nullptr);
            disconnect(playStopButton, &QPushButton::clicked, this, nullptr);
            disconnect(player, &QMediaPlayer::mediaStatusChanged, this, nullptr);

            playback_settings_widget->hide();
            if(hide_widget==playlistwidget)
            {showPlaylist();}
            if(hide_widget==list_of_music)
            {showMusList();}
            isPlaying=false;
            disconnect(off, &QPushButton::clicked, this, nullptr);
            return;
        });

        connect(player, &QMediaPlayer::mediaStatusChanged, this, [=](QMediaPlayer::MediaStatus status){
            handleMediaStatusChanged(status,nameLabel,musics_vec);
        }); //переход на новую песню
    }
    else
    {
        QUrl  currentFilePath = player->source();
        if(mus->getPath() != currentFilePath.toLocalFile()) //проверка на совпадение путей
        {
            player->setSource(QUrl::fromLocalFile(mus->getPath()));
            player->play();
            playStopButton->setIcon(icon_pause);
            nameLabel->setText(mus->getName());
            isPlaying = true;
        }
        else
        {
            if (isPlaying)
            {
                player->pause();
                playStopButton->setIcon(icon_play);

            }
            else
            {
                player->play();
                playStopButton->setIcon(icon_pause);

            }
            isPlaying = !isPlaying;
        }
    }
}

void MainWindow::updateSliderPosition(QSlider *slider) {
    qint64 currentPosition = player->position();
    qint64 duration = player->duration();
    int newPosition = static_cast<int>(currentPosition * slider->maximum() / duration);
    slider->setValue(newPosition);
}

void MainWindow::setPlaybackPosition(int position, QSlider *slider) {
    qint64 duration = player->duration(); // Общая длительность медиа
    qint64 newPosition = position * duration / slider->maximum(); // Новая позиция в миллисекундах
    player->setPosition(newPosition);
}

void MainWindow::handleMediaStatusChanged(QMediaPlayer::MediaStatus status, QLabel *nameLabel, QVector<QMusic*> musics_vec)
{

    if (status == QMediaPlayer::EndOfMedia) {
        if(path_next!="")
        {
            player->setSource(QUrl::fromLocalFile(path_next));
            player->play();
            auto it = std::find_if(musics_vec.begin(),musics_vec.end(),[=](QMusic *a)
            {
                return a->getPath()==path_next;
            });
            int index = std::distance(musics_vec.begin(), it);
            nameLabel->setText(musics_vec[index]->getName());
            path_next="";
        }
        else
        {
            QUrl Url=player->source();
            QString path=Url.toLocalFile();
            auto it = std::find_if(musics_vec.begin(),musics_vec.end(),[=](QMusic *a)
            {
                return a->getPath()==path;
            });
            if (it+1 != musics_vec.end()) {
                int index = std::distance(musics_vec.begin(), it);
                player->setSource(QUrl::fromLocalFile(musics_vec[index+1]->getPath()));
                nameLabel->setText(musics_vec[index+1]->getName());
                player->play();
            } else {
                player->setSource(QUrl::fromLocalFile(musics_vec.first()->getPath()));
                nameLabel->setText(musics_vec.first()->getName());
                player->play();
            }
        }
    }

}

void MainWindow::delet_mus_toPlaulist(QMusic *mus)
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(nullptr, "Подтверждение удаления", "Вы точно хотите удалить эту музыку из плейлиста?",
                                  QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        for(auto &it:playlist_vec_del)
        {
            QVBoxLayout *del=it->getLayout();
            int index = del->indexOf(mus);
            if (index != -1) {
                it->del_music(mus);
                return;
            }
        }
    }
    else return;
}

void MainWindow::delet_mus(QMusic *mus)
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(nullptr, "Подтверждение удаления", "Вы точно хотите удалить эту музыку?",
                                  QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        QUrl Url=player->source();
        QString path_play=Url.toLocalFile();
        auto it = std::find_if(musics_vec.begin(),musics_vec.end(),[=](QMusic *a)
                               {
                                   return a->getPath()==mus->getPath();
                               });
        int index = std::distance(musics_vec.begin(), it);

        if(path_play==mus->getPath())
        {
            player->stop();
            if(index+1<musics_vec.size()-1)
            {
                player->setSource(QUrl::fromLocalFile(musics_vec[index+1]->getPath()));
                nameLabel->setText(musics_vec[index+1]->getName());
            }
            else {
                player->setSource(QUrl::fromLocalFile(musics_vec.first()->getPath()));
                nameLabel->setText(musics_vec.first()->getName());
            }
        }
        QFile file(mus->getPath());
        if (file.exists()) {
            if (file.remove()) {
                qDebug() << "Файл успешно удален.";
            } else {
                qDebug() << "Ошибка удаления файла.";
            }
        }
        else {
            qDebug() << "Файл не существует.";
        }
        for(auto &it:playlist_vec_del)
        {
            QVector<QMusic*> del=it->getVec();
            for(auto &it2:del)
            {
                if(it2->getPath()==mus->getPath())
                {
                    it->del_music(mus);
                }
            }
        }
        // Завершаем текущий запуск программы
        if (index >= 0) {
            musics_vec.remove(index);
        }
        mus->hide();

        player->play();
    } else {
        return;
    }

}

void MainWindow::hide_playList(QPlaylist *playlist, QVBoxLayout *layout)
{
    for (int i = 0; i < layout->count(); ++i) {
        QLayoutItem *item = layout->itemAt(i);
        if (item->widget() != playlist) {
            if (dynamic_cast<QPlaylist*>(item->widget())||dynamic_cast<QPushButton*>(item->widget())) {
                item->widget()->hide();
            }
        }
    }

    playlist_vec=playlist->getVec();

    for(auto &it:playlist_vec)
    {
        connect(it,&QMusic::path_,this, [=](QMusic *mus) {
            if(player->mediaStatus() == QMediaPlayer::PlayingState)
            {
                QUrl Url=player->source();
                QString path_play=Url.toLocalFile();
                if(mus->getPath()==path_play)
                {
                    qint64 currentPosition=player->position();
                    emit disconnect_play();
                    play_mus(mus,playlistwidget,playlist_vec);
                    player->setPosition(currentPosition);
                }
                else
                {
                    emit disconnect_play();
                    play_mus(mus,playlistwidget,playlist_vec);
                }
            }
            else
            {
                play_mus(mus,playlistwidget,playlist_vec);
            }
        });
        connect(it, &QMusic::delet, this, &MainWindow::delet_mus_toPlaulist);

    }

    connect(playlist,&QPlaylist::show_,[=](QPlaylist *playlist){
        for (int i = 0; i < layout->count(); ++i) {
            QLayoutItem *item = layout->itemAt(i);
            if (item->widget() != playlist) {
                if (dynamic_cast<QPlaylist*>(item->widget())||dynamic_cast<QPushButton*>(item->widget())) {
                    item->widget()->show();
                }
            }
        }
    });
}

MainWindow::~MainWindow()
{
    player->stop();
    // Освобождаем память от плеера
    delete player;
}

void MainWindow::readFolder(QString folderPath)
{
    QDir directory(folderPath);

    if (!directory.exists()) {
        qDebug() << "Папка не существует";
        return;
    }

    QStringList fileList = directory.entryList(QDir::Files);

    for (const QString &fileName : fileList) {
        qDebug() << "Название файла:" << fileName;
        MetadataText metadata;
        readMP3Metadata("mus/"+fileName, metadata);
        qDebug() << "Название: " << metadata.title;
        qDebug() << "Артист: " << metadata.artist;
        qDebug() << "Альбом: " << metadata.album;
        qDebug() << "Год: " << metadata.year;
        qDebug() << "Жанр: " << metadata.genre;
        qDebug() << " ";

        QMusic *mus=new QMusic("mus/"+fileName);

        mus->setName(metadata.title);
        mus->setArtist(metadata.artist);
        mus->setAlbum(metadata.album);
        mus->setYear(metadata.year.toInt());
        mus->setGenre(metadata.genre);
        musics_vec.push_back(mus);
    }
}

void MainWindow::read_playlist(QString basePath)
{
    QVector<QMusic*>vec_playlist;
    QPixmap scaledPixmap;
    playlistwidget=new QWidget();

    QPushButton *addPlaylist=new QPushButton("Додати плейлист");
    addPlaylist->setMinimumHeight(30);
    addPlaylist->setMaximumWidth(130);

    playlistlayout=new QVBoxLayout();
    playlistlayout->addWidget(addPlaylist,Qt::AlignHCenter);
    playlistlayout->setSpacing(0);
    QVBoxLayout *virtualLayout=new QVBoxLayout();

    QDir baseDir(basePath);

    if (!baseDir.exists()) {
        qDebug() << "Directory not found:" << basePath;
        return;
    }

    QStringList playlistFolders = baseDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    foreach (const QString &playlistFolder, playlistFolders) {
        QString playlistFolderPath = baseDir.filePath(playlistFolder);
        qDebug() << "Playlist folder:" << playlistFolderPath;

        QFileInfo fileInfo(playlistFolderPath);
        QString namePlaylist = "назва: "+fileInfo.baseName();

        QDir directory(playlistFolderPath);

        if (!directory.exists()) {
            qDebug() << "Папка не существует";
            return;
        }

        QStringList fileList = directory.entryList(QDir::Files);

        for (const QString &fileName : fileList) {
            QFileInfo fileInfo(fileName);


            if (fileInfo.suffix().toLower() == "jpg" || fileInfo.suffix().toLower() == "png") {
                // Этот файл - картинка
                QPixmap pixmap(playlistFolderPath+"/"+fileName);
                scaledPixmap = pixmap.scaled(60, 60, Qt::KeepAspectRatio);
            }
            else if (fileInfo.suffix().toLower() == "txt") {
                QFile file(playlistFolderPath+"/"+fileName);
                if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                    qDebug() << "Ошибка открытия файла";
                    return;
                }
                QTextStream in(&file);
                while (!in.atEnd()) {
                    QString line = in.readLine();
                    for(auto &it:musics_vec)
                    {
                        if(it->getPath()==line)
                        {
                            QMusic *mus= new QMusic(*it);
                            vec_playlist.push_back(mus);
                        }
                    }
                }

                file.close();

            }
            else {
                qDebug() << "Неизвестный файл:" << fileName;
            }
        }
        if(scaledPixmap.isNull()){
            QPixmap pixmap("img/photo.jpg");
            scaledPixmap = pixmap.scaled(60, 60, Qt::KeepAspectRatio);
        }
        QPlaylist *playlist=new QPlaylist(playlistFolderPath,vec_playlist,namePlaylist,playlistwidget);
        playlist->change_photo(scaledPixmap);
        playlistlayout->addWidget(playlist);
        playlist_vec_del.push_back(playlist);
        vec_playlist.clear();
        scaledPixmap = QPixmap();

        connect(playlist, &QPlaylist::hide, this, [=]() {
            hide_playList(playlist, playlistlayout);
        }); //вернуть список плейлистов

        connect(playlist, &QPlaylist::delet,[=](QPlaylist *del){
            QLayoutItem *itemToRemove = playlistlayout->itemAt(playlistlayout->indexOf(del));  //номер в лаяуте
            if (itemToRemove) {
                playlistlayout->removeItem(itemToRemove);
                delete itemToRemove->widget();
                delete itemToRemove;
            }
        }); //удаление плейлиста
    }

    virtualLayout->addLayout(playlistlayout);
    virtualLayout->addStretch();
    playlistwidget->setLayout(virtualLayout);

    connect(addPlaylist,&QPushButton::clicked,[=](){
        playlistwidget->hide();
        QWidget *addWidget= new QWidget();
        addWidget->setMaximumWidth(420);
        QHBoxLayout *addLayout= new QHBoxLayout();
        QVBoxLayout *virtualL= new QVBoxLayout();
        QPushButton *addbut=new QPushButton("Далі");
        addbut->setMaximumSize(100,20);
        QPushButton *off=new QPushButton("Назад");
        off->setMaximumSize(100,20);
        QLineEdit *lineEdit=new QLineEdit();
        lineEdit->setMaximumSize(300,25);
        lineEdit->setMinimumWidth(200);
        lineEdit->setPlaceholderText("Назва");

        addLayout->addWidget(lineEdit);
        addLayout->addWidget(addbut);
        addLayout->addWidget(off);
        virtualL->addLayout(addLayout);
        addWidget->setLayout(virtualL);
        virtualL->addStretch();
        mainlayout->addWidget(addWidget);
        connect(addbut,&QPushButton::clicked,[=](){
            QString name=lineEdit->text();
            mainlayout->removeWidget(addWidget);
            delete addWidget;

            QDir playlistDir("playlist");

            if (playlistDir.mkdir(name)) {
                qDebug() << "Папка" << name << "создана внутри playlist";
                QFile file("playlist/" + name + "/list.txt");
                if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                    file.close();
                    qDebug() << "Текстовый файл успешно создан в папке" << "playlist/" + name;
                } else {
                    qDebug() << "Не удалось открыть файл для записи";
                }
            }
            QVector<QMusic*>vec_playlist;
            QPlaylist *playlist=new QPlaylist("playlist/" + name, vec_playlist, name, playlistwidget);
            playlistlayout->addWidget(playlist,Qt::AlignTop);

            playlistwidget->show();
            connect(playlist, &QPlaylist::hide, this, [=]() {
                hide_playList(playlist, playlistlayout);
            }); //вернуть список плейлистов

            connect(playlist, &QPlaylist::delet,[=](QPlaylist *del){
                QLayoutItem *itemToRemove = playlistlayout->itemAt(playlistlayout->indexOf(del));  //номер в лаяуте
                if (itemToRemove) {
                    playlistlayout->removeItem(itemToRemove);
                    delete itemToRemove->widget();
                    delete itemToRemove;
                }
            });//удаление из лаяута
        });
        connect(off,&QPushButton::clicked,[=](){
            mainlayout->removeWidget(addWidget);
            delete addWidget;
            playlistwidget->show();
        });
    }); //добавить плейлист
}

void MainWindow::addSettingBut()
{
    QToolButton *setting=new QToolButton();
    QIcon icon("img/settings.png");
    setting->setIcon(icon);
    setting->setIconSize(QSize(22, 22));
    setting->setFixedSize(25, 25);
    setting->setStyleSheet("QToolButton { background-color: transparent; border: none;  border-radius: 8px; }"
                           "QToolButton:hover { background-color: #404040; }");

    QMenu *menu = new QMenu(setting);

    QAction *action1 = new QAction("Добавить музыку", setting);
    QAction *action2 = new QAction("Альбомы", setting);
    QAction *action3 = new QAction("Главная", setting);
    QAction *action4 = new QAction("Плейлисты", setting);

    menu->addAction(action1);
    menu->addAction(action2);
    menu->addAction(action3);
    menu->addAction(action4);

    connect(setting, &QToolButton::clicked, [=](){
        if (menu->isVisible()) {
            menu->hide();
        } else {
            // Получение геометрии кнопки
            QRect buttonGeometry = setting->geometry();
            // Расчет позиции для меню
            QPoint menuPos = mapToGlobal(QPoint(buttonGeometry.right(), buttonGeometry.bottom()));
            // Отображение меню
            menu->exec(menuPos);
        }
    });

    connect(action1, &QAction::triggered, [=](){
        QString selectedFilePath = QFileDialog::getOpenFileName(this, "Выберите файл", "", "All Files (*.*)");

        if (!selectedFilePath.isEmpty()) {
            // Путь, куда нужно скопировать файл
            QString destinationPath = "mus/";

            // Получаем имя файла из полного пути
            QString fileName = QFileInfo(selectedFilePath).fileName();

            // Создаем объект файла
            QFile sourceFile(selectedFilePath);
            QFile destinationFile(destinationPath + fileName);

            // Пытаемся скопировать файл
            if (sourceFile.copy(destinationFile.fileName())) {
                qDebug() << "Файл скопирован успешно";
            } else {
                qDebug() << "Ошибка при копировании файла";
            }
        }

        QCoreApplication::exit(MainWindow::EXIT_CODE_REBOOT);
        QString program = QCoreApplication::applicationFilePath();
        QStringList arguments = QCoreApplication::arguments();
        QProcess::startDetached(program, arguments);
    });

    connect(action4, &QAction::triggered, [=](){
        list_of_music->hide();
        change=true;
        playlistwidget->show();
    });

    //connect(action2, &QAction::triggered, this, &QMusic::addToPlaylist);
    connect(action3, &QAction::triggered, [=](){
        change=false;
        playlistwidget->hide();
        list_of_music->show();
    });

    mainlayout->addWidget(setting, Qt::AlignRight);
}
