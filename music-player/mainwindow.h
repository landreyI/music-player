#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "QMusic.h"
#include "QPlaylist.h"
#include <QMainWindow>
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QCoreApplication>
#include <QFileDialog>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QUrl>
#include <QGridLayout>
#include <QSlider>
#include <QTimer>
#include <QCoreApplication>
#include <QProcess>
#include <QLineEdit>
#include <QScrollArea>
#include "AuidioPintWidget.h"
using namespace std;
extern QString path_next;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:

void play_mus(QMusic *mus, QWidget *hide_widget, QVector<QMusic*> musics_vec);
void delet_mus(QMusic *mus);
void delet_mus_toPlaulist(QMusic *mus);
void hide_playList(QPlaylist *playlist, QVBoxLayout *layout);
void handleMediaStatusChanged(QMediaPlayer::MediaStatus status,QLabel *nameLabel, QVector<QMusic*> musics_vec);
void setPlaybackPosition(int position, QSlider *slider);
void updateSliderPosition(QSlider *slider);
private:
bool change=false;
void hidePlaylist(){
    if(change)
    playlistwidget->hide();
}
void showPlaylist(){
    if(change)
    playlistwidget->show();
}
void hideMusList(){
    if(!change)
    list_of_music->hide();
}
void showMusList(){
    if(!change)
    list_of_music->show();
}
void reproduce();
static const int EXIT_CODE_REBOOT = 1000;
void readFolder(QString folderPath);
void read_playlist(QString basePath);
    void addSettingBut();
    //AudioWaveformWidget *waveformWidget;
    QLabel *nameLabel;
    QWidget *mainWidget;
    QVBoxLayout *mainlayout;
    QWidget *playlistwidget;
    QWidget *list_of_music;
    QWidget *playback_settings_widget;
    QVBoxLayout *playlistlayout;
    bool isPlaying = false;
    QPushButton *playStopButton;
    QVector<QMusic*> musics_vec;
    QVector<QMusic*> playlist_vec;
    QVector<QPlaylist*> playlist_vec_del;
    QMediaPlayer *player;
    QPushButton *off;
    QPushButton *hide;
    QPushButton *previous;
    QPushButton *next;
    QSlider *slider;
signals:
    void disconnect_play();
};
#endif // MAINWINDOW_H
