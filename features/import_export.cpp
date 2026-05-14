#include "import_export.h"

const std::unordered_map<QString, std::function<void(const QString&, std::function<void(int row, int col, QString& val)>)>> features::Importer::Importers = {
    {"csv", features::CSVProcessor::Import}
};

const std::unordered_map<QString, std::function<bool(const QDir&, const QString&, const Sheet&)>> features::Exporter::Exporters = {
    {"csv", features::CSVProcessor::Export}
};

void features::CSVProcessor::Import(const QString& path, std::function<void(int row, int col, QString& val)> out) {
    QFile file(path);
    if (file.open(QFile::ReadOnly)) {
        Sheet result;
        QTextStream input(&file);
        ParseText(input, out);
    }
    else {
        throw std::runtime_error("Error on import file " + path.toStdString());
    }
}

bool features::CSVProcessor::Export(const QDir& path, const QString& name, const Sheet& sheet) {
    QFile file(path.filePath(name + ".csv"));
    if (!file.open(QFile::WriteOnly | QFile::Truncate)) {
        return false;
    }
    QTextStream out(&file);
    auto size = sheet.GetPrintableSize();
    for (int i = 0; i < size.rows; ++i) {
        bool first = true;
        for (int j = 0; j < size.cols; ++j) {
            if (first) {
                first = false;
            }
            else {
                out << ';';
            }
            auto cell = sheet.GetCell({.row = i, .col = j});
            auto text = cell->GetText();
            if (cell && !text.empty()) {
                try {
                    size_t pos;
                    std::stod(text, &pos);
                    if (pos == text.size()) {
                        out << QString{text.data()};
                    }
                    else {
                        out << '"' << QString{text.data()} << '"';
                    }
                }
                catch(...) {
                    out << '"' << QString{text.data()} << '"';
                }
            }
        }
        out << '\n';
    }
    return true;
}

void features::CSVProcessor::ParseText(QTextStream& input, std::function<void(int row, int col, QString& val)> out) {
    static std::unordered_set<QChar> separators = {';', ','};
    QString value;
    QString line;
    bool quotes = false;
    int line_index = 0;
    int val_index = 0;
    while (input.readLineInto(&line)) {
        for (int i = 0; i <= line.size(); ++i) {
            if (i == line.size()) {
                if (!quotes) {
                    out(line_index, val_index, value);
                    value.clear();

                    val_index = 0;
                    ++line_index;
                }
                else {
                    value += '\n';
                }
                continue;
            }
            if (line[i] == '"') {
                if (i + 1 < line.size() && line[i + 1] == '"') {
                    value += '"';
                    ++i; // Пропускаем следующий символ
                    continue;
                }
                else {
                    quotes = !quotes;
                }
            }
            else if (separators.count(line[i]) != 0 && !quotes) {
                out(line_index, val_index, value);
                value.clear();
                ++val_index;
            }
            else {
                value += line[i];
            }
        }
    }
}

bool features::Importer::CanImport(const QString& path) {
    QFileInfo file{path};
    return CanImport(file);
}

bool features::Importer::CanImport(const QFileInfo& file) {
    if (file.exists() && Importers.count(file.suffix()) != 0)
        return true;
    return false;
}

void features::Importer::Import(const QString& path, std::function<void(int row, int col, QString& val)> out) {
    QFileInfo file{path};
    if (CanImport(file)) {
        Importers.at(file.suffix())(path, out);
    }
    else {
        throw std::runtime_error("Can't import " + path.toStdString());
    }
}

bool features::Exporter::Export(const QDir& path, const QString& name, const Sheet& sheet, const QString& suffix) {
    return Exporters.at(suffix)(path, name, sheet);
}
