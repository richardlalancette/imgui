//  To parse this JSON data, first install
//
//      Boost     http://www.boost.org
//      json.hpp  https://github.com/nlohmann/json
//
//  Then include this file, and then do
//
//     DesignSystem data = nlohmann::json::parse(jsonString);

#pragma once

#include "json.hpp"

#ifndef NLOHMANN_OPT_HELPER
#define NLOHMANN_OPT_HELPER
namespace nlohmann
{
    template<typename T>
    struct adl_serializer<std::shared_ptr<T>>
    {
        static void to_json(json &j, const std::shared_ptr<T> &opt)
        {
            if (!opt)
                j = nullptr;
            else
                j = *opt;
        }

        static std::shared_ptr<T> from_json(const json &j)
        {
            if (j.is_null())
                return std::unique_ptr<T>();
            else
                return std::unique_ptr<T>(new T(j.get<T>()));
        }
    };
}
#endif

namespace youi
{
    using nlohmann::json;

    inline json get_untyped(const json &j, const char *property)
    {
        if (j.find(property) != j.end())
        {
            return j.at(property).get<json>();
        }
        return json();
    }

    inline json get_untyped(const json &j, std::string property)
    {
        return get_untyped(j, property.data());
    }

    template<typename T>
    inline std::shared_ptr<T> get_optional(const json &j, const char *property)
    {
        if (j.find(property) != j.end())
        {
            return j.at(property).get<std::shared_ptr<T>>();
        }
        return std::shared_ptr<T>();
    }

    template<typename T>
    inline std::shared_ptr<T> get_optional(const json &j, std::string property)
    {
        return get_optional<T>(j, property.data());
    }

    struct FlavorColorColorRgba
    {
        std::shared_ptr<double> a;
        std::shared_ptr<double> b;
        std::shared_ptr<double> g;
        std::shared_ptr<double> r;
    };

    struct FlavorColor
    {
        std::shared_ptr<FlavorColorColorRgba> color_rgba;
        std::shared_ptr<std::string> name;
    };

    struct PurpleMetadata
    {
        std::shared_ptr<std::vector<std::string>> prop;
    };

    struct ColorsFlavor
    {
        std::shared_ptr<std::vector<FlavorColor>> flavor_colors;
        std::shared_ptr<PurpleMetadata> metadata;
        std::shared_ptr<std::string> name;
    };

    struct Colors
    {
        std::shared_ptr<std::vector<ColorsFlavor>> flavors;
    };

    struct Instructions
    {
        std::shared_ptr<std::string> design_system_format;
        std::shared_ptr<std::vector<std::string>> metadata;
    };

    struct Curve
    {
        std::shared_ptr<std::string> curvetype;
        std::shared_ptr<std::vector<nlohmann::json>> points;
    };

    struct MotionFlavor
    {
        std::shared_ptr<Curve> curve;
        std::shared_ptr<int64_t> delay;
        std::shared_ptr<std::string> delayunits;
        std::shared_ptr<std::string> description;
        std::shared_ptr<double> duration;
        std::shared_ptr<std::string> durationunit;
        std::shared_ptr<std::string> name;
        std::shared_ptr<std::string> title;
        std::shared_ptr<std::string> transformation;
    };

    struct Motion
    {
        std::shared_ptr<std::vector<MotionFlavor>> flavors;
    };

    struct FluffyMetadata
    {
        std::shared_ptr<std::vector<std::string>> prop;
    };

    struct TypographyStyleColorRgba
    {
        std::shared_ptr<double> a;
        std::shared_ptr<double> b;
        std::shared_ptr<double> g;
        std::shared_ptr<double> r;
    };

    struct TypographyStyle
    {
        std::shared_ptr<TypographyStyleColorRgba> color_rgba;
        std::shared_ptr<std::string> fontname;
        std::shared_ptr<double> fontsize;
        std::shared_ptr<std::string> name;
    };

    struct TypographyFlavor
    {
        std::shared_ptr<FluffyMetadata> metadata;
        std::shared_ptr<std::string> name;
        std::shared_ptr<std::vector<TypographyStyle>> typography_styles;
    };

    struct Typography
    {
        std::shared_ptr<std::vector<TypographyFlavor>> flavors;
    };

    struct DesignSystem
    {
        std::shared_ptr<Colors> colors;
        std::shared_ptr<std::map<std::string, nlohmann::json>> components;
        std::shared_ptr<Instructions> instructions;
        std::shared_ptr<Motion> motion;
        std::shared_ptr<Typography> typography;
    };
}

namespace nlohmann
{
    void from_json(const json &j, youi::FlavorColorColorRgba &x);
    void to_json(json &j, const youi::FlavorColorColorRgba &x);

    void from_json(const json &j, youi::FlavorColor &x);
    void to_json(json &j, const youi::FlavorColor &x);

    void from_json(const json &j, youi::PurpleMetadata &x);
    void to_json(json &j, const youi::PurpleMetadata &x);

    void from_json(const json &j, youi::ColorsFlavor &x);
    void to_json(json &j, const youi::ColorsFlavor &x);

    void from_json(const json &j, youi::Colors &x);
    void to_json(json &j, const youi::Colors &x);

    void from_json(const json &j, youi::Instructions &x);
    void to_json(json &j, const youi::Instructions &x);

    void from_json(const json &j, youi::Curve &x);
    void to_json(json &j, const youi::Curve &x);

    void from_json(const json &j, youi::MotionFlavor &x);
    void to_json(json &j, const youi::MotionFlavor &x);

    void from_json(const json &j, youi::Motion &x);
    void to_json(json &j, const youi::Motion &x);

    void from_json(const json &j, youi::FluffyMetadata &x);
    void to_json(json &j, const youi::FluffyMetadata &x);

    void from_json(const json &j, youi::TypographyStyleColorRgba &x);
    void to_json(json &j, const youi::TypographyStyleColorRgba &x);

    void from_json(const json &j, youi::TypographyStyle &x);
    void to_json(json &j, const youi::TypographyStyle &x);

    void from_json(const json &j, youi::TypographyFlavor &x);
    void to_json(json &j, const youi::TypographyFlavor &x);

    void from_json(const json &j, youi::Typography &x);
    void to_json(json &j, const youi::Typography &x);

    void from_json(const json &j, youi::DesignSystem &x);
    void to_json(json &j, const youi::DesignSystem &x);

    inline void from_json(const json &j, youi::FlavorColorColorRgba &x)
    {
        x.a = youi::get_optional<double>(j, "a");
        x.b = youi::get_optional<double>(j, "b");
        x.g = youi::get_optional<double>(j, "g");
        x.r = youi::get_optional<double>(j, "r");
    }

    inline void to_json(json &j, const youi::FlavorColorColorRgba &x)
    {
        j = json::object();
        j["a"] = x.a;
        j["b"] = x.b;
        j["g"] = x.g;
        j["r"] = x.r;
    }

    inline void from_json(const json &j, youi::FlavorColor &x)
    {
        x.color_rgba = youi::get_optional<youi::FlavorColorColorRgba>(j, "colorRGBA");
        x.name = youi::get_optional<std::string>(j, "name");
    }

    inline void to_json(json &j, const youi::FlavorColor &x)
    {
        j = json::object();
        j["colorRGBA"] = x.color_rgba;
        j["name"] = x.name;
    }

    inline void from_json(const json &j, youi::PurpleMetadata &x)
    {
        x.prop = youi::get_optional<std::vector<std::string>>(j, "prop");
    }

    inline void to_json(json &j, const youi::PurpleMetadata &x)
    {
        j = json::object();
        j["prop"] = x.prop;
    }

    inline void from_json(const json &j, youi::ColorsFlavor &x)
    {
        x.flavor_colors = youi::get_optional<std::vector<youi::FlavorColor>>(j, "flavorColors");
        x.metadata = youi::get_optional<youi::PurpleMetadata>(j, "metadata");
        x.name = youi::get_optional<std::string>(j, "name");
    }

    inline void to_json(json &j, const youi::ColorsFlavor &x)
    {
        j = json::object();
        j["flavorColors"] = x.flavor_colors;
        j["metadata"] = x.metadata;
        j["name"] = x.name;
    }

    inline void from_json(const json &j, youi::Colors &x)
    {
        x.flavors = youi::get_optional<std::vector<youi::ColorsFlavor>>(j, "flavors");
    }

    inline void to_json(json &j, const youi::Colors &x)
    {
        j = json::object();
        j["flavors"] = x.flavors;
    }

    inline void from_json(const json &j, youi::Instructions &x)
    {
        x.design_system_format = youi::get_optional<std::string>(j, "designSystemFormat");
        x.metadata = youi::get_optional<std::vector<std::string>>(j, "metadata");
    }

    inline void to_json(json &j, const youi::Instructions &x)
    {
        j = json::object();
        j["designSystemFormat"] = x.design_system_format;
        j["metadata"] = x.metadata;
    }

    inline void from_json(const json &j, youi::Curve &x)
    {
        x.curvetype = youi::get_optional<std::string>(j, "curvetype");
        x.points = youi::get_optional<std::vector<json>>(j, "points");
    }

    inline void to_json(json &j, const youi::Curve &x)
    {
        j = json::object();
        j["curvetype"] = x.curvetype;
        j["points"] = x.points;
    }

    inline void from_json(const json &j, youi::MotionFlavor &x)
    {
        x.curve = youi::get_optional<youi::Curve>(j, "curve");
        x.delay = youi::get_optional<int64_t>(j, "delay");
        x.delayunits = youi::get_optional<std::string>(j, "delayunits");
        x.description = youi::get_optional<std::string>(j, "description");
        x.duration = youi::get_optional<double>(j, "duration");
        x.durationunit = youi::get_optional<std::string>(j, "durationunit");
        x.name = youi::get_optional<std::string>(j, "name");
        x.title = youi::get_optional<std::string>(j, "title");
        x.transformation = youi::get_optional<std::string>(j, "transformation");
    }

    inline void to_json(json &j, const youi::MotionFlavor &x)
    {
        j = json::object();
        j["curve"] = x.curve;
        j["delay"] = x.delay;
        j["delayunits"] = x.delayunits;
        j["description"] = x.description;
        j["duration"] = x.duration;
        j["durationunit"] = x.durationunit;
        j["name"] = x.name;
        j["title"] = x.title;
        j["transformation"] = x.transformation;
    }

    inline void from_json(const json &j, youi::Motion &x)
    {
        x.flavors = youi::get_optional<std::vector<youi::MotionFlavor>>(j, "flavors");
    }

    inline void to_json(json &j, const youi::Motion &x)
    {
        j = json::object();
        j["flavors"] = x.flavors;
    }

    inline void from_json(const json &j, youi::FluffyMetadata &x)
    {
        x.prop = youi::get_optional<std::vector<std::string>>(j, "prop");
    }

    inline void to_json(json &j, const youi::FluffyMetadata &x)
    {
        j = json::object();
        j["prop"] = x.prop;
    }

    inline void from_json(const json &j, youi::TypographyStyleColorRgba &x)
    {
        x.a = youi::get_optional<double>(j, "a");
        x.b = youi::get_optional<double>(j, "b");
        x.g = youi::get_optional<double>(j, "g");
        x.r = youi::get_optional<double>(j, "r");
    }

    inline void to_json(json &j, const youi::TypographyStyleColorRgba &x)
    {
        j = json::object();
        j["a"] = x.a;
        j["b"] = x.b;
        j["g"] = x.g;
        j["r"] = x.r;
    }

    inline void from_json(const json &j, youi::TypographyStyle &x)
    {
        x.color_rgba = youi::get_optional<youi::TypographyStyleColorRgba>(j, "colorRGBA");
        x.fontname = youi::get_optional<std::string>(j, "fontname");
        x.fontsize = youi::get_optional<double>(j, "fontsize");
        x.name = youi::get_optional<std::string>(j, "name");
    }

    inline void to_json(json &j, const youi::TypographyStyle &x)
    {
        j = json::object();
        j["colorRGBA"] = x.color_rgba;
        j["fontname"] = x.fontname;
        j["fontsize"] = x.fontsize;
        j["name"] = x.name;
    }

    inline void from_json(const json &j, youi::TypographyFlavor &x)
    {
        x.metadata = youi::get_optional<youi::FluffyMetadata>(j, "metadata");
        x.name = youi::get_optional<std::string>(j, "name");
        x.typography_styles = youi::get_optional<std::vector<youi::TypographyStyle>>(j, "typographyStyles");
    }

    inline void to_json(json &j, const youi::TypographyFlavor &x)
    {
        j = json::object();
        j["metadata"] = x.metadata;
        j["name"] = x.name;
        j["typographyStyles"] = x.typography_styles;
    }

    inline void from_json(const json &j, youi::Typography &x)
    {
        x.flavors = youi::get_optional<std::vector<youi::TypographyFlavor>>(j, "flavors");
    }

    inline void to_json(json &j, const youi::Typography &x)
    {
        j = json::object();
        j["flavors"] = x.flavors;
    }

    inline void from_json(const json &j, youi::DesignSystem &x)
    {
        x.colors = youi::get_optional<youi::Colors>(j, "colors");
        x.components = youi::get_optional<std::map<std::string, json>>(j, "components");
        x.instructions = youi::get_optional<youi::Instructions>(j, "instructions");
        x.motion = youi::get_optional<youi::Motion>(j, "motion");
        x.typography = youi::get_optional<youi::Typography>(j, "typography");
    }

    inline void to_json(json &j, const youi::DesignSystem &x)
    {
        j = json::object();
        j["colors"] = x.colors;
        j["components"] = x.components;
        j["instructions"] = x.instructions;
        j["motion"] = x.motion;
        j["typography"] = x.typography;
    }
}
