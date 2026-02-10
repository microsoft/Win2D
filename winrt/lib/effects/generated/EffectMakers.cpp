// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#include "pch.h"

#include "AlphaMaskEffect.h"
#include "ArithmeticCompositeEffect.h"
#include "AtlasEffect.h"
#include "BlendEffect.h"
#include "BorderEffect.h"
#include "BrightnessEffect.h"
#include "ChromaKeyEffect.h"
#include "ColorManagementEffect.h"
#include "ColorMatrixEffect.h"
#include "ColorSourceEffect.h"
#include "CompositeEffect.h"
#include "ContrastEffect.h"
#include "ConvolveMatrixEffect.h"
#include "CropEffect.h"
#include "CrossFadeEffect.h"
#include "DirectionalBlurEffect.h"
#include "DiscreteTransferEffect.h"
#include "DisplacementMapEffect.h"
#include "DistantDiffuseEffect.h"
#include "DistantSpecularEffect.h"
#include "DpiCompensationEffect.h"
#include "EdgeDetectionEffect.h"
#include "EmbossEffect.h"
#include "ExposureEffect.h"
#include "GammaTransferEffect.h"
#include "GaussianBlurEffect.h"
#include "GrayscaleEffect.h"
#include "HdrToneMapEffect.h"
#include "HighlightsAndShadowsEffect.h"
#include "HueRotationEffect.h"
#include "HueToRgbEffect.h"
#include "InvertEffect.h"
#include "LinearTransferEffect.h"
#include "LuminanceToAlphaEffect.h"
#include "MorphologyEffect.h"
#include "OpacityEffect.h"
#include "OpacityMetadataEffect.h"
#include "PointDiffuseEffect.h"
#include "PointSpecularEffect.h"
#include "PosterizeEffect.h"
#include "PremultiplyEffect.h"
#include "RgbToHueEffect.h"
#include "SaturationEffect.h"
#include "ScaleEffect.h"
#include "SepiaEffect.h"
#include "ShadowEffect.h"
#include "SharpenEffect.h"
#include "SpotDiffuseEffect.h"
#include "SpotSpecularEffect.h"
#include "StraightenEffect.h"
#include "TableTransfer3DEffect.h"
#include "TableTransferEffect.h"
#include "TemperatureAndTintEffect.h"
#include "TileEffect.h"
#include "TintEffect.h"
#include "Transform2DEffect.h"
#include "Transform3DEffect.h"
#include "TurbulenceEffect.h"
#include "UnPremultiplyEffect.h"
#include "VignetteEffect.h"
#include "WhiteLevelAdjustmentEffect.h"


std::pair<IID, CanvasEffect::MakeEffectFunction> CanvasEffect::m_effectMakers[] =
{
    { AlphaMaskEffect::EffectId(),            MakeEffect<AlphaMaskEffect>            },
    { ArithmeticCompositeEffect::EffectId(),  MakeEffect<ArithmeticCompositeEffect>  },
    { AtlasEffect::EffectId(),                MakeEffect<AtlasEffect>                },
    { BlendEffect::EffectId(),                MakeEffect<BlendEffect>                },
    { BorderEffect::EffectId(),               MakeEffect<BorderEffect>               },
    { BrightnessEffect::EffectId(),           MakeEffect<BrightnessEffect>           },
    { ChromaKeyEffect::EffectId(),            MakeEffect<ChromaKeyEffect>            },
    { ColorManagementEffect::EffectId(),      MakeEffect<ColorManagementEffect>      },
    { ColorMatrixEffect::EffectId(),          MakeEffect<ColorMatrixEffect>          },
    { ColorSourceEffect::EffectId(),          MakeEffect<ColorSourceEffect>          },
    { CompositeEffect::EffectId(),            MakeEffect<CompositeEffect>            },
    { ContrastEffect::EffectId(),             MakeEffect<ContrastEffect>             },
    { ConvolveMatrixEffect::EffectId(),       MakeEffect<ConvolveMatrixEffect>       },
    { CropEffect::EffectId(),                 MakeEffect<CropEffect>                 },
    { CrossFadeEffect::EffectId(),            MakeEffect<CrossFadeEffect>            },
    { DirectionalBlurEffect::EffectId(),      MakeEffect<DirectionalBlurEffect>      },
    { DiscreteTransferEffect::EffectId(),     MakeEffect<DiscreteTransferEffect>     },
    { DisplacementMapEffect::EffectId(),      MakeEffect<DisplacementMapEffect>      },
    { DistantDiffuseEffect::EffectId(),       MakeEffect<DistantDiffuseEffect>       },
    { DistantSpecularEffect::EffectId(),      MakeEffect<DistantSpecularEffect>      },
    { DpiCompensationEffect::EffectId(),      MakeEffect<DpiCompensationEffect>      },
    { EdgeDetectionEffect::EffectId(),        MakeEffect<EdgeDetectionEffect>        },
    { EmbossEffect::EffectId(),               MakeEffect<EmbossEffect>               },
    { ExposureEffect::EffectId(),             MakeEffect<ExposureEffect>             },
    { GammaTransferEffect::EffectId(),        MakeEffect<GammaTransferEffect>        },
    { GaussianBlurEffect::EffectId(),         MakeEffect<GaussianBlurEffect>         },
    { GrayscaleEffect::EffectId(),            MakeEffect<GrayscaleEffect>            },
    { HdrToneMapEffect::EffectId(),           MakeEffect<HdrToneMapEffect>           },
    { HighlightsAndShadowsEffect::EffectId(), MakeEffect<HighlightsAndShadowsEffect> },
    { HueRotationEffect::EffectId(),          MakeEffect<HueRotationEffect>          },
    { HueToRgbEffect::EffectId(),             MakeEffect<HueToRgbEffect>             },
    { InvertEffect::EffectId(),               MakeEffect<InvertEffect>               },
    { LinearTransferEffect::EffectId(),       MakeEffect<LinearTransferEffect>       },
    { LuminanceToAlphaEffect::EffectId(),     MakeEffect<LuminanceToAlphaEffect>     },
    { MorphologyEffect::EffectId(),           MakeEffect<MorphologyEffect>           },
    { OpacityEffect::EffectId(),              MakeEffect<OpacityEffect>              },
    { OpacityMetadataEffect::EffectId(),      MakeEffect<OpacityMetadataEffect>      },
    { PointDiffuseEffect::EffectId(),         MakeEffect<PointDiffuseEffect>         },
    { PointSpecularEffect::EffectId(),        MakeEffect<PointSpecularEffect>        },
    { PosterizeEffect::EffectId(),            MakeEffect<PosterizeEffect>            },
    { PremultiplyEffect::EffectId(),          MakeEffect<PremultiplyEffect>          },
    { RgbToHueEffect::EffectId(),             MakeEffect<RgbToHueEffect>             },
    { SaturationEffect::EffectId(),           MakeEffect<SaturationEffect>           },
    { ScaleEffect::EffectId(),                MakeEffect<ScaleEffect>                },
    { SepiaEffect::EffectId(),                MakeEffect<SepiaEffect>                },
    { ShadowEffect::EffectId(),               MakeEffect<ShadowEffect>               },
    { SharpenEffect::EffectId(),              MakeEffect<SharpenEffect>              },
    { SpotDiffuseEffect::EffectId(),          MakeEffect<SpotDiffuseEffect>          },
    { SpotSpecularEffect::EffectId(),         MakeEffect<SpotSpecularEffect>         },
    { StraightenEffect::EffectId(),           MakeEffect<StraightenEffect>           },
    { TableTransfer3DEffect::EffectId(),      MakeEffect<TableTransfer3DEffect>      },
    { TableTransferEffect::EffectId(),        MakeEffect<TableTransferEffect>        },
    { TemperatureAndTintEffect::EffectId(),   MakeEffect<TemperatureAndTintEffect>   },
    { TileEffect::EffectId(),                 MakeEffect<TileEffect>                 },
    { TintEffect::EffectId(),                 MakeEffect<TintEffect>                 },
    { Transform2DEffect::EffectId(),          MakeEffect<Transform2DEffect>          },
    { Transform3DEffect::EffectId(),          MakeEffect<Transform3DEffect>          },
    { TurbulenceEffect::EffectId(),           MakeEffect<TurbulenceEffect>           },
    { UnPremultiplyEffect::EffectId(),        MakeEffect<UnPremultiplyEffect>        },
    { VignetteEffect::EffectId(),             MakeEffect<VignetteEffect>             },
    { WhiteLevelAdjustmentEffect::EffectId(), MakeEffect<WhiteLevelAdjustmentEffect> },

    { GUID_NULL, nullptr }
};
