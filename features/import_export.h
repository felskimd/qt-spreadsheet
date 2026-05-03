#ifndef IMPORT_EXPORT_H
#define IMPORT_EXPORT_H

#include <QDir>
#include <QString>

#include "../spreadsheet/sheet.h"
//#include "../windows/sheet_model.h"

namespace features {

class CSVProcessor {
public:
    CSVProcessor() = delete;
    //static Sheet Import(const QString& path);
    //static void ImportToModel(const QString& path, SheetModel& out);
    static void Import(const QString& path, std::function<void(int row, int col, QString& val)> out);
    static bool Export(const QDir& path, const QString& name, const Sheet& sheet);

private:
    //static void ParseText(QTextStream& input, Sheet& out);
    //static void ParseText(QTextStream& input, SheetModel& out);
    static void ParseText(QTextStream& input, std::function<void(int row, int col, QString& value)> out);
};

class Importer {
public:
    Importer() = delete;
    static bool CanImport(const QString& path);
    static bool CanImport(const QFileInfo& file);
    static void Import(const QString& path, std::function<void(int row, int col, QString& val)> out);
    //const static std::unordered_map<QString, std::function<Sheet(const QString&)>> Importers;
    //const static std::unordered_map<QString, std::function<void(const QString&, SheetModel&)>> ImportersToModel;
    const static std::unordered_map<QString, std::function<void(const QString&, std::function<void(int row, int col, QString& val)>)>> Importers;

private:
};

class Exporter {
public:
    Exporter() = delete;
    static bool Export(const QDir& path, const QString& name, const Sheet& sheet, const QString& suffix);
    const static std::unordered_map<QString, std::function<bool(const QDir&, const QString&, const Sheet&)>> Exporters;

private:
};

} // namespace features

#endif // IMPORT_EXPORT_H
