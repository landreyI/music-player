#ifndef READ_FOLDER_H
#define READ_FOLDER_H

#include <QTextStream>
#include <QByteArray>
#include <QString>
struct MetadataText
{
    QString title;
    QString artist;
    QString album;
    QString year;
    QString genre ;
};

QString readStringFromStream(QDataStream& stream, int length) {
    QByteArray data;
    for (int i = 0; i < length; i++) {
        char c;
        stream.readRawData(&c, 1);
        if (c != '\0') {
            data.append(c);
        }
    }
    return QString::fromUtf8(data);
}

MetadataText readMP3Metadata(const QString& filename, MetadataText& metadata)
{
    QFile mp3File(filename);

    if (!mp3File.open(QIODevice::ReadOnly)) {
        qDebug() << "Не удалось открыть файл.";
        return metadata;
    }

    QDataStream mp3Stream(&mp3File);
    mp3Stream.setByteOrder(QDataStream::BigEndian); // Установка порядка байт для считывания

    char header[10];
    mp3Stream.readRawData(header, 10);

    if (mp3Stream.status() != QDataStream::Ok || strncmp(header, "ID3", 3) != 0) {
        qDebug() << "Файл не содержит заголовок ID3v2.";
        return metadata;
    }

    unsigned char majorVersion = header[3];
    unsigned char minorVersion = header[4];
    unsigned char flags = header[5];

    int tagSize = (header[6] << 21) | (header[7] << 14) | (header[8] << 7) | header[9];

    qDebug() << "Версия ID3v2: " << static_cast<int>(majorVersion) << "." << static_cast<int>(minorVersion);
    qDebug() << "Размер тега: " << tagSize << " байт";

    while (tagSize > 0) {
        char frameHeader[10];
        mp3Stream.readRawData(frameHeader, 10);
        if (mp3Stream.status() != QDataStream::Ok) {
            qDebug() << "Ошибка при чтении фрейма.";
            break;
        }

        QString frameID = QString::fromUtf8(frameHeader, 4);
        int frameSize = (frameHeader[4] << 24) | (frameHeader[5] << 16) | (frameHeader[6] << 8) | frameHeader[7];

        if (frameSize < 1) {
            qDebug() << "Неверный размер фрейма.";
            break;
        }

        if (frameID == "TIT2") {
            metadata.title = readStringFromStream(mp3Stream, frameSize);
        }
        else if (frameID == "TPE1") {
            metadata.artist = readStringFromStream(mp3Stream, frameSize);
        }
        else if (frameID == "TALB") {
            metadata.album = readStringFromStream(mp3Stream, frameSize);
        }
        else if (frameID == "TYER") {
            metadata.year = readStringFromStream(mp3Stream, frameSize);
        }
        else if (frameID == "TCON") {
            metadata.genre = readStringFromStream(mp3Stream, frameSize);
        }
        else {
            mp3Stream.skipRawData(frameSize);
        }

        tagSize -= (10 + frameSize);
    }

    mp3File.close();
    return metadata;
}



#endif // READ_FOLDER_H
