#pragma once

#include "BRDFileBase.h"
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <string>

class XZZPCBFile : public BRDFileBase {
public:
    XZZPCBFile() = default;
    ~XZZPCBFile() = default;

    // Implementation of pure virtual methods
    bool Load(const std::vector<char>& buffer, const std::string& filepath = "") override;
    bool VerifyFormat(const std::vector<char>& buffer) override;

    // Static factory method
    static std::unique_ptr<XZZPCBFile> LoadFromFile(const std::string& filepath);

    // Sample data creation for testing
    void CreateEnhancedSampleData();

private:
    std::unordered_map<uint32_t, std::string> net_dict;
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> diode_dict;
    BRDPoint xy_translation = {0, 0};
    int diode_readings_type = 0;    // Real XZZPCB parsing methods (using original OpenBoardView logic)
    bool ExtractRealComponents(const std::vector<char>& buffer);
    bool ParseXZZPCBOriginal(std::vector<char>& buf);
    void ProcessBlockOriginal(uint8_t block_type, std::vector<char>& block_buf);
    void ParseNetBlockOriginal(std::vector<char>& buf);
    void ParseArcBlockOriginal(std::vector<uint32_t>& buf);
    void ParseLineSegmentBlockOriginal(std::vector<uint32_t>& buf);
    void ParsePartBlockOriginal(std::vector<char>& buf);
    void ParseTestPadBlockOriginal(std::vector<uint8_t>& buf);
    void ParsePostV6(std::vector<char>::iterator v6_pos, std::vector<char>& buf);
    std::vector<std::pair<BRDPoint, BRDPoint>> ConvertArcToSegments(int startAngle, int endAngle, int r, BRDPoint pc);
    void FindXYTranslation();
    void TranslateSegments();
    void TranslatePins();
    void TranslatePoints(BRDPoint& point) const;
    char ReadUtf8Char(char c) const;
    std::string ReadCb2312String(const std::string& str);// Sample data creation for testing
    void CreateRealisticPCBFromSize(int estimated_components);

    // DES decryption
    void DesDecrypt(std::vector<char>& buf);
    void DecryptWithDES(std::vector<char>& buf);

    // Block parsing methods from original OpenBoardView
    void ParseBlock(uint8_t block_type, std::vector<char>& block_buf);
    void ParseArcBlock(std::vector<uint32_t>& buf);
    void ParseLineSegmentBlock(std::vector<uint32_t>& buf);
    void ParsePartBlock(std::vector<char>& buf);
    void ParseTestPadBlock(std::vector<uint8_t>& buf);
    void ParseNetBlock(std::vector<char>& buf);
    bool ParseMainBlocks(std::vector<char>& buffer);
    void ParsePostV6(size_t v6_pos, std::vector<char>& buf);

    // Utility methods
    std::vector<std::pair<BRDPoint, BRDPoint>> ArcToSegments(int startAngle, int endAngle, int r, BRDPoint pc);
    char ReadUtf8Char(char c) const;
    std::string ReadCb2312String(const std::string& str);
    void FindXyTranslation();
    void TranslatePoints(BRDPoint& point) const;
    void TranslateSegments();
    void TranslatePins();
};
