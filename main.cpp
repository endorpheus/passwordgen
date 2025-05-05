#include <QApplication>
#include <QCheckBox>
#include <QClipboard>
#include <QCloseEvent>
#include <QComboBox>
#include <QCryptographicHash>
#include <QFont>
#include <QFontDatabase>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMainWindow>
#include <QMenu>
#include <QMessageBox>
#include <QProgressBar>
#include <QPushButton>
#include <QSettings>
#include <QSlider>
#include <QString>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>
#include <QList>
#include <QDialog>
#include <QScreen>
#include <random>
#include <QWheelEvent>
#include <QMouseEvent>
#include <cmath>

// Custom secure password field with additional security features
class SecurePasswordField : public QLineEdit {
    Q_OBJECT
public:
    SecurePasswordField(QWidget* parent = nullptr) : QLineEdit(parent) {
        setMouseTracking(true);
        
        // Set up clipboard clear timer
        clipboardTimer = new QTimer(this);
        connect(clipboardTimer, &QTimer::timeout, this, &SecurePasswordField::clearClipboard);
    }
    
    ~SecurePasswordField() {
        // Clear sensitive data when destroyed
        setText(QString(text().size(), 'X'));
        clear();
    }
    
    void copyToClipboardSecurely(const QString &text, int timeoutSeconds = 30) {
        QClipboard *clipboard = QGuiApplication::clipboard();
        clipboard->setText(text);
        
        // Start timer to clear clipboard after specified seconds
        clipboardTimer->start(timeoutSeconds * 1000);
    }

signals:
    void wheelScrolled(bool forward);
    void middleButtonClicked();
    
protected:
    void wheelEvent(QWheelEvent* event) override {
        bool scrollForward = event->angleDelta().y() > 0;
        emit wheelScrolled(scrollForward);
        event->accept();
    }
    
    void mousePressEvent(QMouseEvent* event) override {
        if (event->button() == Qt::MiddleButton) {
            emit middleButtonClicked();
            event->accept();
        } else {
            QLineEdit::mousePressEvent(event);
        }
    }
    
private slots:
    void clearClipboard() {
        QClipboard *clipboard = QGuiApplication::clipboard();
        if (clipboard->text() == text()) {
            clipboard->clear();
        }
        clipboardTimer->stop();
    }
    
private:
    QTimer *clipboardTimer;
};

class PasswordHistoryDialog : public QDialog {
    Q_OBJECT
public:
    PasswordHistoryDialog(const QList<QString>& history, QWidget* parent = nullptr)
        : QDialog(parent) {
        setWindowTitle("Password History");
        setMinimumWidth(400);
        
        auto* layout = new QVBoxLayout(this);
        layout->setContentsMargins(4, 4, 4, 4);
        layout->setSpacing(4);
        
        historyList = new QListWidget(this);
        for (int i = history.size() - 1; i >= 0; --i) {
            historyList->addItem(history[i]);
        }
        
        if (!history.isEmpty()) {
            historyList->setCurrentRow(0);
        }
        
        layout->addWidget(historyList);
        
        auto* buttonLayout = new QHBoxLayout();
        buttonLayout->setSpacing(4);
        
        selectButton = new QPushButton("Select");
        cancelButton = new QPushButton("Cancel");
        
        buttonLayout->addWidget(selectButton);
        buttonLayout->addWidget(cancelButton);
        layout->addLayout(buttonLayout);
        
        connect(selectButton, &QPushButton::clicked, this, &QDialog::accept);
        connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
        connect(historyList, &QListWidget::itemDoubleClicked, this, &QDialog::accept);
    }
    
    QString getSelectedPassword() const {
        if (historyList->currentItem()) {
            return historyList->currentItem()->text();
        }
        return QString();
    }
    
private:
    QListWidget* historyList;
    QPushButton* selectButton;
    QPushButton* cancelButton;
};

class PasswordGenerator : public QMainWindow {
    Q_OBJECT

public:
    PasswordGenerator(QWidget *parent = nullptr) : QMainWindow(parent) {
        setWindowTitle("Secure Password Generator");
        
        // Initialize settings with proper format
        initSettings();
        
        // Initialize cryptographically secure random generator
        initSecureRandom();
        
        auto *centralWidget = new QWidget(this);
        auto *layout = new QVBoxLayout(centralWidget);
        layout->setContentsMargins(6, 6, 6, 6);
        layout->setSpacing(4);
        
        // Password field with centered text
        passwordField = new SecurePasswordField();
        passwordField->setPlaceholderText("Generated password will appear here...");
        passwordField->setAlignment(Qt::AlignCenter);
        
        QFont defaultFont("Arial", 12, QFont::Bold);
        defaultFont.setStyleHint(QFont::SansSerif, QFont::PreferAntialias);
        passwordField->setFont(defaultFont);
        
        // Create a properly sized strength meter
        strengthMeter = new QProgressBar();
        strengthMeter->setTextVisible(true);
        strengthMeter->setRange(0, 100);
        strengthMeter->setFixedHeight(25);  // Increased height to properly fit text
        
        // Set a more readable font for the strength meter
        QFont meterFont = strengthMeter->font();
        meterFont.setPointSize(10);
        strengthMeter->setFont(meterFont);
        
        // Add password field and strength meter to a container
        auto *passwordContainer = new QVBoxLayout();
        passwordContainer->setContentsMargins(0, 0, 0, 0);
        passwordContainer->setSpacing(4);  // Slightly more space between elements
        passwordContainer->addWidget(passwordField);
        passwordContainer->addWidget(strengthMeter);
        layout->addLayout(passwordContainer);
        
        // Create tabs (main options and advanced options)
        auto *tabLayout = new QTabWidget();
        
        // Main tab for basic generation
        auto *basicTab = new QWidget();
        auto *basicLayout = new QVBoxLayout(basicTab);
        basicLayout->setContentsMargins(4, 4, 4, 4);
        basicLayout->setSpacing(4);
        
        // Password length controls
        auto *lengthLayout = new QHBoxLayout();
        lengthLayout->setSpacing(4);
        auto *lengthLabel = new QLabel("Length:");
        lengthLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
        
        lengthValue = new QLabel("20");
        lengthValue->setFixedWidth(20);
        
        lengthSlider = new QSlider(Qt::Horizontal);
        lengthSlider->setMinimum(12);
        lengthSlider->setMaximum(64);
        lengthSlider->setValue(20);
        lengthSlider->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        
        lengthLayout->addWidget(lengthLabel);
        lengthLayout->addWidget(lengthSlider);
        lengthLayout->addWidget(lengthValue);
        basicLayout->addLayout(lengthLayout);
        
        // Character set options in a compact grid
        auto *charsetLayout = new QGridLayout();
        charsetLayout->setHorizontalSpacing(4);
        charsetLayout->setVerticalSpacing(2);
        
        includeUppercase = new QCheckBox("Uppercase");
        includeLowercase = new QCheckBox("Lowercase");
        includeDigits = new QCheckBox("Digits");
        includeSpecial = new QCheckBox("Special");
        
        includeUppercase->setChecked(true);
        includeLowercase->setChecked(true);
        includeDigits->setChecked(true);
        includeSpecial->setChecked(true);
        
        charsetLayout->addWidget(includeUppercase, 0, 0);
        charsetLayout->addWidget(includeLowercase, 0, 1);
        charsetLayout->addWidget(includeDigits, 1, 0);
        charsetLayout->addWidget(includeSpecial, 1, 1);
        basicLayout->addLayout(charsetLayout);
        
        // Button layout - main operations
        auto *buttonLayout = new QHBoxLayout();
        buttonLayout->setSpacing(4);
        
        generateButton = new QPushButton("Generate");
        removeSpecialCharsButton = new QPushButton("Remove Special");
        undoButton = new QPushButton("Undo");
        
        // Set compact button sizes
        generateButton->setFixedHeight(30);
        removeSpecialCharsButton->setFixedHeight(30);
        undoButton->setFixedHeight(30);
        
        undoButton->setEnabled(false);
        
        buttonLayout->addWidget(generateButton);
        buttonLayout->addWidget(removeSpecialCharsButton);
        buttonLayout->addWidget(undoButton);
        basicLayout->addLayout(buttonLayout);
        
        // Advanced tab with additional settings
        auto *advancedTab = new QWidget();
        auto *advancedLayout = new QVBoxLayout(advancedTab);
        advancedLayout->setContentsMargins(4, 4, 4, 4);
        advancedLayout->setSpacing(4);
        
        // Font selection in advanced tab
        auto *fontLayout = new QHBoxLayout();
        fontLayout->setSpacing(4);
        auto *fontLabel = new QLabel("Font:");
        fontLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
        
        fontComboBox = new QComboBox();
        fontComboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        
        QFontDatabase fontDatabase;
        const QStringList fontFamilies = fontDatabase.families();
        fontComboBox->addItems(fontFamilies);
        
        int arialIndex = fontComboBox->findText("Arial", Qt::MatchContains);
        if (arialIndex != -1) {
            fontComboBox->setCurrentIndex(arialIndex);
        }
        
        fontLayout->addWidget(fontLabel);
        fontLayout->addWidget(fontComboBox);
        advancedLayout->addLayout(fontLayout);
        
        // Security options
        enforceMinimumChars = new QCheckBox("Enforce minimum of each character type");
        avoidSimilarChars = new QCheckBox("Avoid similar characters (1, l, I, 0, O)");
        autoClearClipboard = new QCheckBox("Auto-clear clipboard (30s)");
        
        enforceMinimumChars->setChecked(true);
        avoidSimilarChars->setChecked(false);
        autoClearClipboard->setChecked(true);
        
        advancedLayout->addWidget(enforceMinimumChars);
        advancedLayout->addWidget(avoidSimilarChars);
        advancedLayout->addWidget(autoClearClipboard);
        
        // Add "Save Settings" button to advanced tab
        saveSettingsButton = new QPushButton("Save Current Settings as Default");
        advancedLayout->addWidget(saveSettingsButton);
        
        // Add "Reset to Defaults" button
        resetSettingsButton = new QPushButton("Reset to Factory Defaults");
        advancedLayout->addWidget(resetSettingsButton);
        
        advancedLayout->addStretch();
        
        // Add tabs to the layout
        tabLayout->addTab(basicTab, "Basic");
        tabLayout->addTab(advancedTab, "Advanced");
        layout->addWidget(tabLayout);
        
        // Set up connections
        connect(lengthSlider, &QSlider::valueChanged, this, &PasswordGenerator::updateLengthLabel);
        connect(fontComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), 
                this, &PasswordGenerator::updateFont);
        connect(generateButton, &QPushButton::clicked, this, &PasswordGenerator::generateNewPassword);
        connect(removeSpecialCharsButton, &QPushButton::clicked, this, &PasswordGenerator::removeSpecialChars);
        connect(undoButton, &QPushButton::clicked, this, &PasswordGenerator::undoPassword);
        connect(passwordField, &SecurePasswordField::wheelScrolled, this, &PasswordGenerator::handleWheelScroll);
        connect(passwordField, &SecurePasswordField::middleButtonClicked, this, &PasswordGenerator::showHistoryDialog);
        connect(passwordField, &QLineEdit::textChanged, this, &PasswordGenerator::updatePasswordStrength);
        
        // Connect settings buttons
        connect(saveSettingsButton, &QPushButton::clicked, this, &PasswordGenerator::saveSettingsWithConfirmation);
        connect(resetSettingsButton, &QPushButton::clicked, this, &PasswordGenerator::resetSettings);
        
        // Connections for auto-saving settings on change
        connect(lengthSlider, &QSlider::valueChanged, this, &PasswordGenerator::autoSaveSettings);
        connect(includeUppercase, &QCheckBox::toggled, this, &PasswordGenerator::autoSaveSettings);
        connect(includeLowercase, &QCheckBox::toggled, this, &PasswordGenerator::autoSaveSettings);
        connect(includeDigits, &QCheckBox::toggled, this, &PasswordGenerator::autoSaveSettings);
        connect(includeSpecial, &QCheckBox::toggled, this, &PasswordGenerator::autoSaveSettings);
        connect(enforceMinimumChars, &QCheckBox::toggled, this, &PasswordGenerator::autoSaveSettings);
        connect(avoidSimilarChars, &QCheckBox::toggled, this, &PasswordGenerator::autoSaveSettings);
        connect(autoClearClipboard, &QCheckBox::toggled, this, &PasswordGenerator::autoSaveSettings);
        connect(fontComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), 
                this, &PasswordGenerator::autoSaveSettings);
        
        setCentralWidget(centralWidget);
                
        // Set default size before loading settings (in case no settings exist yet)
        QScreen *screen = QGuiApplication::primaryScreen();
        QRect screenGeometry = screen->availableGeometry();
        int width = qMin(450, screenGeometry.width() - 100);
        int height = qMin(350, screenGeometry.height() - 100);
        resize(width, height);
        
        // Center the window on screen
        move(screenGeometry.center() - rect().center());
        
        // Now load saved settings (this will override the default size if settings exist)
        loadSettings();
        
        // Initialize with a password
        generateNewPassword();
        
        // Initialize history browsing index
        currentHistoryIndex = -1;
    }
    
    ~PasswordGenerator() {
        // Clear sensitive data when destroyed
        clearPasswordHistory();
        
        // Final save of settings when closing
        saveSettings();
    }

protected:
    void closeEvent(QCloseEvent *event) override {
        // Save all settings including window position/size when closing
        saveSettings();
        QMainWindow::closeEvent(event);
    }

private slots:
    void updateLengthLabel(int value) {
        lengthValue->setText(QString::number(value));
    }
    
    void updateFont(int) {
        QFont newFont(fontComboBox->currentText(), 12, QFont::Bold);
        passwordField->setFont(newFont);
    }
    
    void generateNewPassword() {
        // Save current password to history
        saveToHistory(passwordField->text());
        
        // Generate a new secure password
        QString password = generateSecurePassword(
            lengthSlider->value(),
            includeUppercase->isChecked(),
            includeLowercase->isChecked(),
            includeDigits->isChecked(),
            includeSpecial->isChecked(),
            enforceMinimumChars->isChecked(),
            avoidSimilarChars->isChecked()
        );
        
        passwordField->setText(password);
        
        // Copy to clipboard with security measures if enabled
        if (autoClearClipboard->isChecked()) {
            passwordField->copyToClipboardSecurely(password, 30);
        } else {
            QClipboard *clipboard = QGuiApplication::clipboard();
            clipboard->setText(password);
        }
        
        // Reset history browsing index
        currentHistoryIndex = -1;
    }
    
    void removeSpecialChars() {
        // Save the current password for undo history
        saveToHistory(passwordField->text());
        
        // Get current password and remove special characters
        QString currentPassword = passwordField->text();
        QString cleanPassword;
        
        // Keep only alphanumeric characters
        for (const QChar &ch : currentPassword) {
            if (ch.isLetterOrNumber()) {
                cleanPassword.append(ch);
            }
        }
        
        // Update the field if we have a non-empty result
        if (!cleanPassword.isEmpty()) {
            passwordField->setText(cleanPassword);
            
            // Copy to clipboard with security measures if enabled
            if (autoClearClipboard->isChecked()) {
                passwordField->copyToClipboardSecurely(cleanPassword, 30);
            } else {
                QClipboard *clipboard = QGuiApplication::clipboard();
                clipboard->setText(cleanPassword);
            }
        }
        
        // Reset history browsing index
        currentHistoryIndex = -1;
    }
    
    void undoPassword() {
        if (!passwordHistory.isEmpty()) {
            // We're already at the beginning of history, get the most recent item
            if (currentHistoryIndex <= 0 || currentHistoryIndex >= passwordHistory.size()) {
                currentHistoryIndex = 0;
            }
            
            QString previousPassword = passwordHistory[currentHistoryIndex];
            passwordField->setText(previousPassword);
            
            // Copy to clipboard with security measures if enabled
            if (autoClearClipboard->isChecked()) {
                passwordField->copyToClipboardSecurely(previousPassword, 30);
            } else {
                QClipboard *clipboard = QGuiApplication::clipboard();
                clipboard->setText(previousPassword);
            }
            
            // Move to next history item for the next undo
            currentHistoryIndex++;
            
            // Disable undo button if we've reached the end of history
            undoButton->setEnabled(currentHistoryIndex < passwordHistory.size());
        }
    }
    
    void handleWheelScroll(bool forward) {
        if (passwordHistory.isEmpty()) {
            return;
        }
        
        if (currentHistoryIndex < 0) {
            // First wheel scroll - initialize index
            currentHistoryIndex = forward ? passwordHistory.size() - 1 : 0;
        } else {
            // Adjust index based on scroll direction
            if (forward) {
                // Scroll backward in history (older passwords)
                currentHistoryIndex = qMin(currentHistoryIndex + 1, passwordHistory.size() - 1);
            } else {
                // Scroll forward in history (newer passwords)
                currentHistoryIndex = qMax(currentHistoryIndex - 1, 0);
            }
        }
        
        // Set the password from history
        QString historicalPassword = passwordHistory[currentHistoryIndex];
        passwordField->setText(historicalPassword);
        
        // Copy to clipboard with security measures if enabled
        if (autoClearClipboard->isChecked()) {
            passwordField->copyToClipboardSecurely(historicalPassword, 30);
        } else {
            QClipboard *clipboard = QGuiApplication::clipboard();
            clipboard->setText(historicalPassword);
        }
        
        // Update undo button state
        undoButton->setEnabled(true);
    }
    
    void showHistoryDialog() {
        if (passwordHistory.isEmpty()) {
            return;
        }
        
        PasswordHistoryDialog dialog(passwordHistory, this);
        if (dialog.exec() == QDialog::Accepted) {
            QString selectedPassword = dialog.getSelectedPassword();
            if (!selectedPassword.isEmpty()) {
                passwordField->setText(selectedPassword);
                
                // Copy to clipboard with security measures if enabled
                if (autoClearClipboard->isChecked()) {
                    passwordField->copyToClipboardSecurely(selectedPassword, 30);
                } else {
                    QClipboard *clipboard = QGuiApplication::clipboard();
                    clipboard->setText(selectedPassword);
                }
            }
        }
    }
    
    void updatePasswordStrength() {
        QString password = passwordField->text();
        if (password.isEmpty()) {
            strengthMeter->setValue(0);
            strengthMeter->setFormat("No Password");
            return;
        }
        
        int score = calculatePasswordStrength(password);
        strengthMeter->setValue(score);
        
        if (score < 30) {
            strengthMeter->setStyleSheet("QProgressBar::chunk { background-color: red; }");
            strengthMeter->setFormat("Very Weak");
        } else if (score < 50) {
            strengthMeter->setStyleSheet("QProgressBar::chunk { background-color: orange; }");
            strengthMeter->setFormat("Weak");
        } else if (score < 70) {
            strengthMeter->setStyleSheet("QProgressBar::chunk { background-color: yellow; }");
            strengthMeter->setFormat("Moderate");
        } else if (score < 90) {
            strengthMeter->setStyleSheet("QProgressBar::chunk { background-color: lightgreen; }");
            strengthMeter->setFormat("Strong");
        } else {
            strengthMeter->setStyleSheet("QProgressBar::chunk { background-color: green; }");
            strengthMeter->setFormat("Very Strong");
        }
    }
    
    // Settings management methods
    void initSettings() {
        // Set organization and application name for QSettings
        QCoreApplication::setOrganizationName("SecureTools");
        QCoreApplication::setApplicationName("PasswordGenerator");
        
        // Force settings to use INI format on all platforms for better compatibility
        QSettings::setDefaultFormat(QSettings::IniFormat);
        
        // Initialize settings
        QSettings settings;
        settings.sync();
    }
    
    void loadSettings() {
        QSettings settings;
        
        // Load window geometry and state if available
        if (settings.contains("windowGeometry")) {
            restoreGeometry(settings.value("windowGeometry").toByteArray());
        }
        
        if (settings.contains("windowState")) {
            restoreState(settings.value("windowState").toByteArray());
        }
        
        // Load position and size explicitly as backup
        if (settings.contains("windowX") && settings.contains("windowY")) {
            int x = settings.value("windowX").toInt();
            int y = settings.value("windowY").toInt();
            move(x, y);
        }
        
        if (settings.contains("windowWidth") && settings.contains("windowHeight")) {
            int width = settings.value("windowWidth").toInt();
            int height = settings.value("windowHeight").toInt();
            resize(width, height);
        }
        
        // Load password generation settings
        lengthSlider->setValue(settings.value("passwordLength", 20).toInt());
        
        // Character set options - load with defaults if not found
        includeUppercase->setChecked(settings.value("includeUppercase", true).toBool());
        includeLowercase->setChecked(settings.value("includeLowercase", true).toBool());
        includeDigits->setChecked(settings.value("includeDigits", true).toBool());
        includeSpecial->setChecked(settings.value("includeSpecial", true).toBool());
        
        // Security options - load with defaults if not found
        // Explicitly handling avoidSimilarChars
        bool avoidSimilar = settings.value("avoidSimilarChars", false).toBool();
        avoidSimilarChars->setChecked(avoidSimilar);
        
        enforceMinimumChars->setChecked(settings.value("enforceMinimumChars", true).toBool());
        autoClearClipboard->setChecked(settings.value("autoClearClipboard", true).toBool());
        
        // Load font if available
        QString fontName = settings.value("fontName", "Arial").toString();
        int fontIndex = fontComboBox->findText(fontName, Qt::MatchContains);
        if (fontIndex != -1) {
            fontComboBox->setCurrentIndex(fontIndex);
            QFont font(fontName, 12, QFont::Bold);
            passwordField->setFont(font);
        }
        
        // Update UI to reflect loaded settings
        updateLengthLabel(lengthSlider->value());
    }
    
    void saveSettings() {
        // Create a new QSettings object to save the settings
        QSettings settings;
        
        // Save window state and geometry
        settings.setValue("windowGeometry", saveGeometry());
        settings.setValue("windowState", saveState());
        
        // Also save window position and size explicitly
        settings.setValue("windowX", x());
        settings.setValue("windowY", y());
        settings.setValue("windowWidth", width());
        settings.setValue("windowHeight", height());
        
        // Save password generation settings
        settings.setValue("passwordLength", lengthSlider->value());
        
        // Character set options
        settings.setValue("includeUppercase", includeUppercase->isChecked());
        settings.setValue("includeLowercase", includeLowercase->isChecked());
        settings.setValue("includeDigits", includeDigits->isChecked());
        settings.setValue("includeSpecial", includeSpecial->isChecked());
        
        // Security options - Explicitly handle the problematic checkbox
        bool avoidSimilar = avoidSimilarChars->isChecked();
        settings.setValue("avoidSimilarChars", avoidSimilar);
        
        settings.setValue("enforceMinimumChars", enforceMinimumChars->isChecked());
        settings.setValue("autoClearClipboard", autoClearClipboard->isChecked());
        
        // Font settings
        settings.setValue("fontName", fontComboBox->currentText());
        
        // Ensure settings are written immediately to disk
        settings.sync();
    }
    
    void saveSettingsWithConfirmation() {
        saveSettings();
        QMessageBox::information(this, "Settings Saved", 
                               "Your current settings have been saved as the default.");
    }
    
    void resetSettings() {
        // Ask for confirmation
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Reset Settings", 
                                     "Are you sure you want to reset all settings to defaults?",
                                     QMessageBox::Yes|QMessageBox::No);
        
        if (reply == QMessageBox::Yes) {
            // Reset to default values
            lengthSlider->setValue(20);
            
            includeUppercase->setChecked(true);
            includeLowercase->setChecked(true);
            includeDigits->setChecked(true);
            includeSpecial->setChecked(true);
            
            enforceMinimumChars->setChecked(true);
            avoidSimilarChars->setChecked(false);
            autoClearClipboard->setChecked(true);
            
            // Reset font to Arial
            int arialIndex = fontComboBox->findText("Arial", Qt::MatchContains);
            if (arialIndex != -1) {
                fontComboBox->setCurrentIndex(arialIndex);
                QFont font("Arial", 12, QFont::Bold);
                passwordField->setFont(font);
            }
            
            // Save these default settings
            saveSettings();
            
            QMessageBox::information(this, "Settings Reset", 
                                   "All settings have been reset to factory defaults.");
        }
    }
    
    void autoSaveSettings() {
        // This slot is called whenever settings are changed
        saveSettings();
    }
    
private:
    SecurePasswordField *passwordField;
    QProgressBar *strengthMeter;
    QSlider *lengthSlider;
    QLabel *lengthValue;
    QComboBox *fontComboBox;
    QCheckBox *includeUppercase;
    QCheckBox *includeLowercase;
    QCheckBox *includeDigits;
    QCheckBox *includeSpecial;
    QCheckBox *enforceMinimumChars;
    QCheckBox *avoidSimilarChars;
    QCheckBox *autoClearClipboard;
    QPushButton *generateButton;
    QPushButton *removeSpecialCharsButton;
    QPushButton *undoButton;
    QPushButton *saveSettingsButton;
    QPushButton *resetSettingsButton;
    
    QList<QString> passwordHistory;
    int currentHistoryIndex;
    
    // Cryptographically secure random number generator
    std::random_device rd;
    std::mt19937_64 secureGenerator;
    
    void initSecureRandom() {
        // Seed with high-quality random data
        std::array<unsigned int, std::mt19937_64::state_size> seedData;
        std::random_device rd;
        std::generate(seedData.begin(), seedData.end(), std::ref(rd));
        
        std::seed_seq seq(seedData.begin(), seedData.end());
        secureGenerator.seed(seq);
    }
    
    void saveToHistory(const QString &password) {
        if (!password.isEmpty()) {
            passwordHistory.prepend(password);
            while (passwordHistory.size() > 10) {
                // Securely clear the oldest password
                QString &oldPassword = passwordHistory.last();
                oldPassword.fill('X');  // Overwrite with 'X'
                passwordHistory.removeLast();
            }
            undoButton->setEnabled(!passwordHistory.isEmpty());
        }
    }
    
    void clearPasswordHistory() {
        // Securely clear all passwords in history
        for (QString &password : passwordHistory) {
            password.fill('X');  // Overwrite with 'X'
        }
        passwordHistory.clear();
    }
    
    QString generateSecurePassword(int length, 
                              bool useUpper, 
                              bool useLower, 
                              bool useDigits, 
                              bool useSpecial,
                              bool enforceMinimum,
                              bool avoidSimilar) {
        // Ensure at least one character set is selected
        if (!useUpper && !useLower && !useDigits && !useSpecial) {
            // Default to lowercase if nothing selected
            useLower = true;
        }
        
        // Define character sets
        const QString upperChars = avoidSimilar ? "ABCDEFGHJKLMNPQRSTUVWXYZ" : "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        const QString lowerChars = avoidSimilar ? "abcdefghijkmnopqrstuvwxyz" : "abcdefghijklmnopqrstuvwxyz";
        const QString digitChars = avoidSimilar ? "23456789" : "0123456789";
        const QString specialChars = "!@#$%^&*()-_=+[]{};:,.<>?/";
        
        // Combine selected character sets
        QString chars;
        if (useUpper) chars += upperChars;
        if (useLower) chars += lowerChars;
        if (useDigits) chars += digitChars;
        if (useSpecial) chars += specialChars;
        
        // If enforcing minimum characters of each type
        if (enforceMinimum) {
            // Make sure length is sufficient for minimum requirements
            int requiredLength = 0;
            if (useUpper) requiredLength++;
            if (useLower) requiredLength++;
            if (useDigits) requiredLength++;
            if (useSpecial) requiredLength++;
            
            if (length < requiredLength) {
                length = requiredLength;
                lengthSlider->setValue(length);
            }
            
            // Generate password with at least one of each required character type
            QString password;
            
            // First add one of each required type
            if (useUpper) {
                std::uniform_int_distribution<int> dist(0, upperChars.length() - 1);
                password.append(upperChars.at(dist(secureGenerator)));
            }
            
            if (useLower) {
                std::uniform_int_distribution<int> dist(0, lowerChars.length() - 1);
                password.append(lowerChars.at(dist(secureGenerator)));
            }
            
            if (useDigits) {
                std::uniform_int_distribution<int> dist(0, digitChars.length() - 1);
                password.append(digitChars.at(dist(secureGenerator)));
            }
            
            if (useSpecial) {
                std::uniform_int_distribution<int> dist(0, specialChars.length() - 1);
                password.append(specialChars.at(dist(secureGenerator)));
            }
            
            // Fill the rest randomly
            while (password.length() < length) {
                std::uniform_int_distribution<int> dist(0, chars.length() - 1);
                password.append(chars.at(dist(secureGenerator)));
            }
            
            // Shuffle the password to avoid predictable placement
            std::shuffle(password.begin(), password.end(), secureGenerator);
            
            return password;
        } else {
            // Generate completely random password
            QString password;
            std::uniform_int_distribution<int> dist(0, chars.length() - 1);
            
            for (int i = 0; i < length; ++i) {
                password.append(chars.at(dist(secureGenerator)));
            }
            
            return password;
        }
    }
    
    int calculatePasswordStrength(const QString &password) {
        // Basic zxcvbn-inspired password strength evaluation
        int score = 0;
        
        // Length contribution (up to 40 points)
        score += qMin(40, password.length() * 2);
        
        // Character variety contribution (up to 30 points)
        bool hasLower = false, hasUpper = false, hasDigit = false, hasSpecial = false;
        for (const QChar &ch : password) {
            if (ch.isLower()) hasLower = true;
            else if (ch.isUpper()) hasUpper = true;
            else if (ch.isDigit()) hasDigit = true;
            else hasSpecial = true;
        }
        
        int charTypeCount = (hasLower ? 1 : 0) + (hasUpper ? 1 : 0) + 
                            (hasDigit ? 1 : 0) + (hasSpecial ? 1 : 0);
        score += charTypeCount * 7.5;
        
        // Entropy approximation (up to 30 points)
        double entropy = 0;
        if (hasLower) entropy += 26;
        if (hasUpper) entropy += 26;
        if (hasDigit) entropy += 10;
        if (hasSpecial) entropy += 33;
        
        entropy = log2(entropy) * password.length();
        score += qMin(30.0, entropy / 4.0);
        
        return qMin(100, score);
    }
};

// Need to include this since we're using Q_OBJECT
#include "main.moc"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    PasswordGenerator window;
    window.show();
    return app.exec();
}