#ifndef AUIDIOPINTWIDGET_H
#define AUIDIOPINTWIDGET_H
#include <QApplication>
#include <QWidget>
#include <QAudioDecoder>
#include <QAudioBuffer>
#include <QPainter>
#include <QFile>
#include <QDebug>
#include <QIODevice>

class AudioWaveformWidget : public QWidget {
    Q_OBJECT
public:
    AudioWaveformWidget(QWidget* parent = nullptr) : QWidget(parent), audioDecoder(nullptr) {
        setMinimumSize(600,200);

        connect(&audioDecoder, &QAudioDecoder::bufferReady, this, &AudioWaveformWidget::onBufferReady);
        connect(&audioDecoder, &QAudioDecoder::finished, this, &AudioWaveformWidget::onFinished);
    }

    void loadMp3File(const QString& filePath) {
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly)) {
            qDebug() << "Failed to open MP3 file:" << file.errorString();
            return;
        }

        qDebug() << "Starting audio decoding for file:" << filePath;
        QIODevice* ioDevice = &file; // Получаем указатель на QIODevice
        audioDecoder.setSourceDevice(ioDevice);
        audioDecoder.start();
    }

protected:
    void paintEvent(QPaintEvent* event) override {
        int h = height();
        if (h <= 0) {
            return; // Просто выходим из функции, если высота некорректна
        }
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        QColor bgColor("#303030");
        painter.fillRect(rect(), bgColor);  // Заливка фона
        QColor waveColor("#FFA500");
        painter.setPen(QPen(waveColor, 2));  // Установка цвета и толщины для рисования волн
        painter.translate(0, h / 2);
        // Проверяем, есть ли данные для рисования
        if (waveforms.isEmpty() || waveforms.size() < 2) {
            return;  // Пропускаем пустые волны
        }
        for (int i = 1; i < waveforms.size(); ++i) {
            // Проверяем, что значения индексов и волновых данных в допустимых пределах
            if (i < waveforms.size()) {
                int value1 = waveforms[i - 1] * h / 2;
                int value2 = waveforms[i] * h / 2;
                // Проверяем, что значения не NaN и не бесконечности и не выходят за пределы допустимых
                painter.drawLine(i - 1, value1, i, value2);
            }
        }
    }

private slots:
    void onBufferReady() {
        QAudioBuffer buffer = audioDecoder.read();
        if (buffer.isValid()) { // Проверяем, является ли буфер действительным
            waveforms = calculateWaveform(buffer);
            repaint();
        } else {
            qDebug() << "Invalid audio buffer.";
        }
    }

    void onFinished() {
        qDebug() << "Audio decoding finished.";
        // Обработка завершения декодирования аудио
    }

private:
    QAudioDecoder audioDecoder;
    QVector<qreal> waveforms;

    QVector<qreal> calculateWaveform(const QAudioBuffer& buffer) {
        // Расчет волновой формы из аудио-данных
        waveforms.clear();
        const qint16* data = buffer.constData<qint16>();
        const int sampleCount = buffer.sampleCount();
        QVector<qreal> newWaveform(sampleCount);

        for (int i = 0; i < sampleCount; ++i) {
            qreal value = static_cast<qreal>(data[i]) / SHRT_MAX;  // Нормализация значений
            // Проверка и коррекция значений, если они выходят за пределы -1.0 и 1.0
            if (value < -1.0) {
                value = -1.0;
            } else if (value > 1.0) {
                value = 1.0;
            }
            newWaveform[i] = value;
        }
        return newWaveform;
    }
};
#endif // AUIDIOPINTWIDGET_H
