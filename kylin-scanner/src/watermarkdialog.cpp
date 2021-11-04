#include "watermarkdialog.h"

WatermarkDialog::WatermarkDialog(QWidget *parent) : QDialog(parent)
  , m_titleLable (new QLabel())
  , m_closeButton (new QPushButton())
  , m_titleHBoxLayout (new QHBoxLayout())
  , m_infoTextLabel (new QLabel())
  , m_watermarkLineEdit (new QLineEdit())
  , m_watermarkHBoxLayout (new QHBoxLayout())
  , m_cancelButton (new QPushButton())
  , m_confirmButton (new QPushButton)
  , m_buttonsHBoxLayout (new QHBoxLayout())
  , m_mainVBoxLayout (new QVBoxLayout())
{
    initWindow();

    initLayout();

    initConnect();
}

void WatermarkDialog::initWindow()
{
    MotifWmHints hints;
    hints.flags = MWM_HINTS_FUNCTIONS | MWM_HINTS_DECORATIONS;
    hints.functions = MWM_FUNC_ALL;
    hints.decorations = MWM_DECOR_BORDER;
    XAtomHelper::getInstance()->setWindowMotifHint(winId(), hints);

    setWindowTitle (tr("Scanner"));
    setFixedSize(QSize(380, 176));
}

void WatermarkDialog::initLayout()
{
    m_titleLable->setText(tr("Scanner"));

    m_closeButton->setIcon (QIcon::fromTheme (ICON_THEME_CLOSE));
    m_closeButton->setToolTip(tr("Close"));
    m_closeButton->setFixedSize(30, 30);
    m_closeButton->setIconSize (QSize(16, 16));
    m_closeButton->setProperty("isWindowButton", 0x2);
    m_closeButton->setProperty("useIconHighlightEffect", 0x8);
    m_closeButton->setFlat(true);

    m_titleHBoxLayout->setSpacing(0);
    m_titleHBoxLayout->addSpacing(12);
    m_titleHBoxLayout->addWidget(m_titleLable);
    m_titleHBoxLayout->addStretch();
    m_titleHBoxLayout->addWidget(m_closeButton);
    m_titleHBoxLayout->setContentsMargins(0, 5, 4, 5);

    m_infoTextLabel->setText(tr("Add watermark"));
    QFont ft;
    ft.setPixelSize(14);
    m_infoTextLabel->setFont(ft);
    m_infoTextLabel->setFixedSize(QSize(56, 22));

    m_watermarkLineEdit->setText("");
    m_watermarkLineEdit->setFixedSize(QSize(268, 36));

    m_watermarkHBoxLayout->setSpacing(0);
    m_watermarkHBoxLayout->addSpacing(24);
    m_watermarkHBoxLayout->addWidget(m_infoTextLabel);
    m_watermarkHBoxLayout->addSpacing(8);
    m_watermarkHBoxLayout->addWidget(m_watermarkLineEdit);
    m_watermarkHBoxLayout->setContentsMargins(0, 0, 24, 0);

    m_cancelButton->setText(tr("Cancel"));
    m_cancelButton->setFixedSize(QSize(96, 36));

    m_confirmButton->setText(tr("Confirm"));
    m_confirmButton->setFixedSize(QSize(96, 36));

    m_buttonsHBoxLayout->setSpacing(0);
    m_buttonsHBoxLayout->addSpacing(152);
    m_buttonsHBoxLayout->addWidget(m_cancelButton);
    m_buttonsHBoxLayout->addSpacing(12);
    m_buttonsHBoxLayout->addWidget(m_confirmButton);
    m_buttonsHBoxLayout->setContentsMargins(0, 0, 24, 24);

    m_mainVBoxLayout->setSpacing(0);
    m_mainVBoxLayout->addLayout(m_titleHBoxLayout);
    m_mainVBoxLayout->addSpacing(16);
    m_mainVBoxLayout->addLayout(m_watermarkHBoxLayout);
    m_mainVBoxLayout->addSpacing(24);
    m_mainVBoxLayout->addLayout(m_buttonsHBoxLayout);
    m_mainVBoxLayout->setContentsMargins(0, 0, 0, 0);

    this->setLayout(m_mainVBoxLayout);
}

void WatermarkDialog::initConnect()
{
    connect(m_closeButton, &QPushButton::clicked, this, &WatermarkDialog::close);
    connect(m_cancelButton, &QPushButton::clicked, this, &WatermarkDialog::reject);
    connect(m_confirmButton, &QPushButton::clicked, this, &WatermarkDialog::confirmButtonClickedSlot);

}

QString WatermarkDialog::getLineEditText()
{
    return m_watermarkLineEdit->text();
}

void WatermarkDialog::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    QPainterPath rectPath;
    rectPath.addRoundedRect(this->rect(), 0, 0);
    QStyleOption opt;
    opt.init(this);

    QColor mainColor;
    mainColor = opt.palette.color(QPalette::Window);

    p.fillPath(rectPath, QBrush(mainColor));
}

void WatermarkDialog::confirmButtonClickedSlot()
{
    if (m_watermarkLineEdit->text() == "") {
        this->reject();
    } else {
        this->accept();
    }

}
