#include "about.h"
#include "ui_about.h"


AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::KYCAboutDialog)
    , style_settings (new QGSettings(ORG_UKUI_STYLE))
    , icon_theme_settings (new QGSettings(ORG_UKUI_STYLE))
{
    ui->setupUi(this);

    initWindow();

    initLayout();

    initStyle();

    initConnect();
}


void AboutDialog::titlebar_icon_theme_changed(QString)
{
    setWindowIcon(QIcon::fromTheme("kylin-scanner"));
    ui->labelLogo->setPixmap(QIcon::fromTheme("kylin-scanner").pixmap(ui->labelLogo->size()));
    ui->labelIcon->setPixmap(QIcon::fromTheme("kylin-scanner").pixmap(96, 96));
}

void AboutDialog::titlebar_style_changed(QString)
{
    QPalette pal = QApplication::style()->standardPalette();
    QPalette pp = QApplication::style()->standardPalette();
    QColor c;
    QString text =
        tr("Scanner is an interface-friendly scanner, which could be also used as one-click beautification, intelligent correction and text recognition tools.");

    c.setRed(231);
    c.setBlue(231);
    c.setGreen(231);
    int redBackground = pal.background().color().red();
    int blueBackground = pal.background().color().blue();
    int greenBackground = pal.background().color().green();
    if ((redBackground == blueBackground) && (redBackground == greenBackground) && redBackground > 200) {
        // light theme
        pal.setColor(QPalette::Background, QColor("#FFFFFF"));
        ui->labelSupport->setText(tr("Service & Support : ")
                                  + "<a href=\"mailto://support@kylinos.cn\" style=\"color:rgba(0,0,0,1)\">"
                                  + "support@kylinos.cn</a>");

        ui->textEdit->setText("<body style=\"background:#FFFFFF;\">"
                              + QString("<p style=\"color: %1\">").arg(pal.windowText().color().name(QColor::HexRgb))
                              + text
                              + "</p></body>");
        setPalette(pal);
    } else {
        // dark theme
        setPalette(pal);
        ui->labelSupport->setText(tr("Service & Support : ")
                                  + "<a href=\"mailto://support@kylinos.cn\" style=\"color:rgba(255,255,255,1)\">"
                                  + "support@kylinos.cn</a>");
        ui->textEdit->setText(QString("<body style=\"background:%1;\">") .arg(pal.background().color().name(
                                                                                  QColor::HexRgb))
                              + QString("<p style=\"color: %1\">").arg(pal.windowText().color().name(QColor::HexRgb))
                              + text
                              + "</p></body>");
    }

    ui->labelVersion->setPalette(pp);
    ui->textEdit->setPalette(pp);
    ui->labelSupport->setPalette(pp);
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

void AboutDialog::initWindow()
{
    MotifWmHints hints;
    hints.flags = MWM_HINTS_FUNCTIONS | MWM_HINTS_DECORATIONS;
    hints.functions = MWM_FUNC_ALL;
    hints.decorations = MWM_DECOR_BORDER;
    XAtomHelper::getInstance()->setWindowMotifHint(winId(), hints);

    setWindowModality(Qt::ApplicationModal);
    setWindowTitle(tr("About"));
    setWindowIcon(QIcon::fromTheme("kylin-scanner"));
}

void AboutDialog::initLayout()
{
    QString appVersion = QCoreApplication::applicationVersion();
    ui->labelVersion->setText(tr("Version: " ) + appVersion);

    ui->labelLogo->setPixmap(QIcon::fromTheme("kylin-scanner").pixmap(ui->labelLogo->size()));
    ui->labelIcon->setPixmap(QIcon::fromTheme("kylin-scanner").pixmap(96, 96));

    ui->labelName->setText(tr("Scanner"));
    ui->labelTitle->setText(tr("Scanner"));

    ui->btnClose->setIcon(QIcon::fromTheme("window-close-symbolic"));
    ui->btnClose->setProperty("isWindowButton", 0x2);
    ui->btnClose->setProperty("useIconHighlightEffect", 0x8);
    ui->btnClose->setIconSize(QSize(16, 16));
    ui->btnClose->installEventFilter(this);
    ui->btnClose->setFlat(true);

    ui->labelSupport->setContextMenuPolicy(Qt::NoContextMenu); // no right click menu
}

void AboutDialog::initStyle()
{
    stylelist << STYLE_NAME_KEY_DARK << STYLE_NAME_KEY_BLACK;
    iconthemelist << ICON_THEME_KEY_BASIC << ICON_THEME_KEY_CLASSICAL << ICON_THEME_KEY_DEFAULT;

    QPalette pal = QApplication::style()->standardPalette();
    QPalette pp = QApplication::style()->standardPalette();
    QColor c;
    QString text =
        tr("Scanner is an interface-friendly scanner, which could be also used as one-click beautification, intelligent correction and text recognition tools.");

    c.setRed(245);
    c.setBlue(245);
    c.setGreen(245);

    // (245, 245, 245)
    int redBackground = pal.background().color().red();
    int blueBackground = pal.background().color().blue();
    int greenBackground = pal.background().color().green();
    //if (c == pal.background().color()) {
    if ((redBackground == blueBackground) && (redBackground == greenBackground) && redBackground > 200) {
        // light theme
        pal.setColor(QPalette::Background, QColor("#FFFFFF"));
        ui->labelSupport->setText(tr("Service & Support : ")
                                  + QString("<a href=\"mailto://support@kylinos.cn\" style=\"color:%1\">").arg(pal.windowText().color().name(QColor::HexRgb))
                                  + "support@kylinos.cn</a>");

        ui->textEdit->setText("<body style=\"background:#FFFFFF;\">"
                              + QString("<p style=\"color: %1\">").arg(pal.windowText().color().name(QColor::HexRgb))
                              + text
                              + "</p></body>");
        setPalette(pal);
    } else {
        // dark theme
        setPalette(pal);
        ui->labelSupport->setText(tr("Service & Support : ")
                                  //+ "<a href=\"mailto://support@kylinos.cn\" style=\"color:rgb(255,255,255)\">"
                                  + QString("<a href=\"mailto://support@kylinos.cn\" style=\"color:%1\">").arg(pal.windowText().color().name(QColor::HexRgb))
                                  + "support@kylinos.cn</a>");
        ui->textEdit->setText(QString("<body style=\"background:%1;\">") .arg(pal.background().color().name(
                                                                                  QColor::HexRgb))
                              + QString("<p style=\"color: %1\">").arg(pal.windowText().color().name(QColor::HexRgb))
                              + text
                              + "</p></body>");
    }
    //setFontStyle();

    ui->labelVersion->setPalette(pp);
    ui->textEdit->setPalette(pp);
    ui->labelSupport->setPalette(pp);
}

void AboutDialog::initConnect()
{
    connect(ui->btnClose, SIGNAL(clicked()), this, SLOT(hide()));
    connect(ui->labelSupport, &QLabel::linkActivated, [ = ](QString s) {
        QUrl url(s);
        QDesktopServices::openUrl(url);
    });
    connect(style_settings, SIGNAL(changed(QString)), this, SLOT(titlebar_style_changed(QString)));
    connect(icon_theme_settings, SIGNAL(changed(QString)), this, SLOT(titlebar_icon_theme_changed(QString)));
}

void AboutDialog::setFontStyle()
{
    QFont f = ui->labelTitle->font();
    f.setPixelSize(14);
    ui->labelTitle->setFont(f);

    f.setPixelSize(18);
    ui->labelName->setFont(f);

    f.setPixelSize(14);
    f.setFamily(font().family());

    ui->labelVersion->setFont(f);
    ui->textEdit->setFont(f);
    ui->labelSupport->setFont(f);
}

void AboutDialog::moveCenter()
{
    QScreen *screen = QGuiApplication::primaryScreen ();
    QRect screenRect =  screen->availableGeometry();
    this->move(screenRect.width() / 2, screenRect.height() / 2);
}
