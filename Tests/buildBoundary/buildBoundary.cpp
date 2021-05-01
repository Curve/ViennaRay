#include <embree3/rtcore.h>
#include <rtGeometry.hpp>
#include <rtBoundary.hpp>
#include <rtBoundCondition.hpp>
#include <rtTestAsserts.hpp>
#include <rtUtil.hpp>

int main()
{
    using NumericType = double;
    constexpr int D = 3;
    NumericType eps = 1e-6;

    NumericType gridDelta;
    std::vector<rtTriple<NumericType>> points;
    std::vector<rtTriple<NumericType>> normals;
    rtInternal::readGridFromFile("./../Resources/sphereGrid3D_R1.dat", gridDelta, points, normals);

    auto device = rtcNewDevice("");

    rtGeometry<NumericType, D> geometry;
    geometry.initGeometry(device, points, normals, gridDelta);

    {
        // build boundary in y and z directions
        auto boundingBox = geometry.getBoundingBox();
        auto traceSetting = rtInternal::getTraceSettings(rtTraceDirection::POS_X);
        auto boundary = rtBoundary<NumericType, D>(traceSetting);
        boundingBox[1][0] += gridDelta;
        boundary.initBoundary(device, boundingBox);

        // assert bounding box is ordered
        RAYTEST_ASSERT(boundingBox[0][0] < boundingBox[1][0])
        RAYTEST_ASSERT(boundingBox[0][1] < boundingBox[1][1])
        RAYTEST_ASSERT(boundingBox[0][2] < boundingBox[1][2])

        // assert boundary is extended in x direction
        RAYTEST_ASSERT_ISCLOSE(boundingBox[1][0], (1 + gridDelta), eps)

        // assert boundary normal vectors are perpendicular to x direction
        auto xplane = rtTriple<NumericType>{1., 0., 0.};
        for (size_t i = 0; i < 8; i++)
        {
            auto normal = boundary.getPrimNormal(i);
            RAYTEST_ASSERT_ISNORMAL(normal, xplane, eps)
        }
        boundary.releaseGeometry();
    }

    {
        // build boundary in x and z directions
        auto traceSetting = rtInternal::getTraceSettings(rtTraceDirection::POS_Y);
        auto boundary = rtBoundary<NumericType, D>(traceSetting);
        auto boundingBox = geometry.getBoundingBox();
        boundingBox[1][1] += gridDelta;
        boundary.initBoundary(device, boundingBox);

        // assert bounding box is ordered
        RAYTEST_ASSERT(boundingBox[0][0] < boundingBox[1][0])
        RAYTEST_ASSERT(boundingBox[0][1] < boundingBox[1][1])
        RAYTEST_ASSERT(boundingBox[0][2] < boundingBox[1][2])

        // assert boundary is extended in y direction
        RAYTEST_ASSERT_ISCLOSE(boundingBox[1][1], (1 + gridDelta), eps)

        // assert boundary normal vectors are perpendicular to y direction
        auto yplane = rtTriple<NumericType>{0., 1., 0.};
        for (size_t i = 0; i < 8; i++)
        {
            auto normal = boundary.getPrimNormal(i);
            RAYTEST_ASSERT_ISNORMAL(normal, yplane, eps)
        }
        boundary.releaseGeometry();
    }

    {
        // build boundary in x and y directions
        auto traceSetting = rtInternal::getTraceSettings(rtTraceDirection::POS_Z);
        auto boundary = rtBoundary<NumericType, D>(traceSetting);
        auto boundingBox = geometry.getBoundingBox();
        boundingBox[1][2] += gridDelta;
        boundary.initBoundary(device, boundingBox);

        // assert bounding box is ordered
        RAYTEST_ASSERT(boundingBox[0][0] < boundingBox[1][0])
        RAYTEST_ASSERT(boundingBox[0][1] < boundingBox[1][1])
        RAYTEST_ASSERT(boundingBox[0][2] < boundingBox[1][2])

        // assert boundary is extended in x direction
        RAYTEST_ASSERT_ISCLOSE(boundingBox[1][2], (1 + gridDelta), eps)

        // assert boundary normal vectors are perpendicular to x direction
        auto zplane = rtTriple<NumericType>{0., 0., 1.};
        for (size_t i = 0; i < 8; i++)
        {
            auto normal = boundary.getPrimNormal(i);
            RAYTEST_ASSERT_ISNORMAL(normal, zplane, eps)
        }
        boundary.releaseGeometry();
    }

    rtcReleaseDevice(device);
    return 0;
}