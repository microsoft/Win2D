// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#include "pch.h"

#include "ArithmeticCompositeEffect.h"
#include "AtlasEffect.h"
#include "BlendEffect.h"
#include "BorderEffect.h"
#include "BrightnessEffect.h"
#include "ColorManagementEffect.h"
#include "ColorMatrixEffect.h"
#include "ColorSourceEffect.h"
#include "CompositeEffect.h"
#include "ConvolveMatrixEffect.h"
#include "CropEffect.h"
#include "DirectionalBlurEffect.h"
#include "DiscreteTransferEffect.h"
#include "DisplacementMapEffect.h"
#include "DistantDiffuseEffect.h"
#include "DistantSpecularEffect.h"
#include "DpiCompensationEffect.h"
#include "GammaTransferEffect.h"
#include "GaussianBlurEffect.h"
#include "HueRotationEffect.h"
#include "LinearTransferEffect.h"
#include "LuminanceToAlphaEffect.h"
#include "MorphologyEffect.h"
#include "OpacityMetadataEffect.h"
#include "PointDiffuseEffect.h"
#include "PointSpecularEffect.h"
#include "PremultiplyEffect.h"
#include "SaturationEffect.h"
#include "ScaleEffect.h"
#include "ShadowEffect.h"
#include "SpotDiffuseEffect.h"
#include "SpotSpecularEffect.h"
#include "TableTransferEffect.h"
#include "TileEffect.h"
#include "Transform2DEffect.h"
#include "Transform3DEffect.h"
#include "TurbulenceEffect.h"
#include "UnPremultiplyEffect.h"

#if (defined _WIN32_WINNT_WIN10) && (WINVER >= _WIN32_WINNT_WIN10)

#include "ChromaKeyEffect.h"
#include "ContrastEffect.h"
#include "EdgeDetectionEffect.h"
#include "EmbossEffect.h"
#include "ExposureEffect.h"
#include "GrayscaleEffect.h"
#include "HighlightsAndShadowsEffect.h"
#include "HueToRgbEffect.h"
#include "InvertEffect.h"
#include "PosterizeEffect.h"
#include "RgbToHueEffect.h"
#include "SepiaEffect.h"
#include "SharpenEffect.h"
#include "StraightenEffect.h"
#include "TableTransfer3DEffect.h"
#include "TemperatureAndTintEffect.h"
#include "VignetteEffect.h"

#endif // _WIN32_WINNT_WIN10


std::pair<IID, CanvasEffect::MakeEffectFunction> CanvasEffect::m_effectMakers[] =
{
    { ArithmeticCompositeEffect::EffectId(),  MakeEffect<ArithmeticCompositeEffect>  },
    { AtlasEffect::EffectId(),                MakeEffect<AtlasEffect>                },
    { BlendEffect::EffectId(),                MakeEffect<BlendEffect>                },
    { BorderEffect::EffectId(),               MakeEffect<BorderEffect>               },
    { BrightnessEffect::EffectId(),           MakeEffect<BrightnessEffect>           },
    { ColorManagementEffect::EffectId(),      MakeEffect<ColorManagementEffect>      },
    { ColorMatrixEffect::EffectId(),          MakeEffect<ColorMatrixEffect>          },
    { ColorSourceEffect::EffectId(),          MakeEffect<ColorSourceEffect>          },
    { CompositeEffect::EffectId(),            MakeEffect<CompositeEffect>            },
    { ConvolveMatrixEffect::EffectId(),       MakeEffect<ConvolveMatrixEffect>       },
    { CropEffect::EffectId(),                 MakeEffect<CropEffect>                 },
    { DirectionalBlurEffect::EffectId(),      MakeEffect<DirectionalBlurEffect>      },
    { DiscreteTransferEffect::EffectId(),     MakeEffect<DiscreteTransferEffect>     },
    { DisplacementMapEffect::EffectId(),      MakeEffect<DisplacementMapEffect>      },
    { DistantDiffuseEffect::EffectId(),       MakeEffect<DistantDiffuseEffect>       },
    { DistantSpecularEffect::EffectId(),      MakeEffect<DistantSpecularEffect>      },
    { DpiCompensationEffect::EffectId(),      MakeEffect<DpiCompensationEffect>      },
    { GammaTransferEffect::EffectId(),        MakeEffect<GammaTransferEffect>        },
    { GaussianBlurEffect::EffectId(),         MakeEffect<GaussianBlurEffect>         },
    { HueRotationEffect::EffectId(),          MakeEffect<HueRotationEffect>          },
    { LinearTransferEffect::EffectId(),       MakeEffect<LinearTransferEffect>       },
    { LuminanceToAlphaEffect::EffectId(),     MakeEffect<LuminanceToAlphaEffect>     },
    { MorphologyEffect::EffectId(),           MakeEffect<MorphologyEffect>           },
    { OpacityMetadataEffect::EffectId(),      MakeEffect<OpacityMetadataEffect>      },
    { PointDiffuseEffect::EffectId(),         MakeEffect<PointDiffuseEffect>         },
    { PointSpecularEffect::EffectId(),        MakeEffect<PointSpecularEffect>        },
    { PremultiplyEffect::EffectId(),          MakeEffect<PremultiplyEffect>          },
    { SaturationEffect::EffectId(),           MakeEffect<SaturationEffect>           },
    { ScaleEffect::EffectId(),                MakeEffect<ScaleEffect>                },
    { ShadowEffect::EffectId(),               MakeEffect<ShadowEffect>               },
    { SpotDiffuseEffect::EffectId(),          MakeEffect<SpotDiffuseEffect>          },
    { SpotSpecularEffect::EffectId(),         MakeEffect<SpotSpecularEffect>         },
    { TableTransferEffect::EffectId(),        MakeEffect<TableTransferEffect>        },
    { TileEffect::EffectId(),                 MakeEffect<TileEffect>                 },
    { Transform2DEffect::EffectId(),          MakeEffect<Transform2DEffect>          },
    { Transform3DEffect::EffectId(),          MakeEffect<Transform3DEffect>          },
    { TurbulenceEffect::EffectId(),           MakeEffect<TurbulenceEffect>           },
    { UnPremultiplyEffect::EffectId(),        MakeEffect<UnPremultiplyEffect>        },

#if (defined _WIN32_WINNT_WIN10) && (WINVER >= _WIN32_WINNT_WIN10)

    { ChromaKeyEffect::EffectId(),            MakeEffect<ChromaKeyEffect>            },
    { ContrastEffect::EffectId(),             MakeEffect<ContrastEffect>             },
    { EdgeDetectionEffect::EffectId(),        MakeEffect<EdgeDetectionEffect>        },
    { EmbossEffect::EffectId(),               MakeEffect<EmbossEffect>               },
    { ExposureEffect::EffectId(),             MakeEffect<ExposureEffect>             },
    { GrayscaleEffect::EffectId(),            MakeEffect<GrayscaleEffect>            },
    { HighlightsAndShadowsEffect::EffectId(), MakeEffect<HighlightsAndShadowsEffect> },
    { HueToRgbEffect::EffectId(),             MakeEffect<HueToRgbEffect>             },
    { InvertEffect::EffectId(),               MakeEffect<InvertEffect>               },
    { PosterizeEffect::EffectId(),            MakeEffect<PosterizeEffect>            },
    { RgbToHueEffect::EffectId(),             MakeEffect<RgbToHueEffect>             },
    { SepiaEffect::EffectId(),                MakeEffect<SepiaEffect>                },
    { SharpenEffect::EffectId(),              MakeEffect<SharpenEffect>              },
    { StraightenEffect::EffectId(),           MakeEffect<StraightenEffect>           },
    { TableTransfer3DEffect::EffectId(),      MakeEffect<TableTransfer3DEffect>      },
    { TemperatureAndTintEffect::EffectId(),   MakeEffect<TemperatureAndTintEffect>   },
    { VignetteEffect::EffectId(),             MakeEffect<VignetteEffect>             },

#endif // _WIN32_WINNT_WIN10

    { GUID_NULL, nullptr }
};
