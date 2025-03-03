# Cereal

My personal JSON serialization library. Maybe xml too

# Configure Build
In projects/cereal/premake5.lua you can set the serialization precision for floats in doubles
```lua
    defines {
        -- Example setting double precision to 5
        "CEREAL_DBL_PRECISION=5",
        -- Example setting float precision to 2
        "CEREAL_FLT_PRECISION=2"

        -- defaults are std::numeric_limits<double>::digits10 and std::numeric_limits<float>::digits10
    }
```