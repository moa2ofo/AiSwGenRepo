// CEEDLING NOTICE: This generated file only to be consumed for test runner creation

#include "utExecutionAndResults/utUnderTest/build/vendor/unity/src/unity.h"
#include "utExecutionAndResults/utUnderTest/src/voltMonRun.h"
#include "mock_VoltMonitoring_priv.h"
#include "mock_VoltMonitoring_cfg.h"

VoltMon_Context_t VoltMon_Ctx;
void setUp(void)
{
    VoltMon_Ctx.state = VOLT_MON_STATE_NORMAL;
    VoltMon_Ctx.uvActivationTimer_ms = 0u;
    VoltMon_Ctx.ovActivationTimer_ms = 0u;
    VoltMon_Ctx.deactivationTimer_ms = 0u;
}

void tearDown(void)
{
}

void test_voltMonRun_NormalState_VoltageWithinBand_RemainsNormal(void)
{
    setUp();
    uint16_t voltage = 10000u;
    VoltMon_ReadVoltageProject_mV_CMockExpectAndReturn(46, voltage);

    VoltMon_GetUnderOn_mV_CMockExpectAndReturn(49, 8000u);
    VoltMon_GetUnderOff_mV_CMockExpectAndReturn(50, 8500u);
    VoltMon_GetOverOn_mV_CMockExpectAndReturn(51, 12500u);
    VoltMon_GetOverOff_mV_CMockExpectAndReturn(52, 13000u);

    voltMonRun(10u);

    UnityAssertEqualNumber((UNITY_INT)((VOLT_MON_STATE_NORMAL)), (UNITY_INT)((VoltMon_Ctx.state)), (
   ((void *)0)
   ), (UNITY_UINT)(57), UNITY_DISPLAY_STYLE_INT);
    UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT16)((0u)), (UNITY_INT)(UNITY_UINT16)((VoltMon_Ctx.uvActivationTimer_ms)), (
   ((void *)0)
   ), (UNITY_UINT)(58), UNITY_DISPLAY_STYLE_UINT16);
    UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT16)((0u)), (UNITY_INT)(UNITY_UINT16)((VoltMon_Ctx.ovActivationTimer_ms)), (
   ((void *)0)
   ), (UNITY_UINT)(59), UNITY_DISPLAY_STYLE_UINT16);
    UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT16)((0u)), (UNITY_INT)(UNITY_UINT16)((VoltMon_Ctx.deactivationTimer_ms)), (
   ((void *)0)
   ), (UNITY_UINT)(60), UNITY_DISPLAY_STYLE_UINT16);
}

void test_voltMonRun_NormalState_VoltageToUnderVoltage(void)
{
    setUp();

    for(int i = 0; i < (VoltMon_ActivationTime_ms / 10u)+1; i++)
{
        VoltMon_ReadVoltageProject_mV_CMockExpectAndReturn(74, (VoltMon_ThresholdUnder_mV));
        VoltMon_GetUnderOn_mV_CMockExpectAndReturn(75, 8000u);
        VoltMon_GetUnderOff_mV_CMockExpectAndReturn(76, 8500u);
        VoltMon_GetOverOn_mV_CMockExpectAndReturn(77, 12500u);
        VoltMon_GetOverOff_mV_CMockExpectAndReturn(78, 13000u);

        voltMonRun(10u);
    }

    UnityAssertEqualNumber((UNITY_INT)((VOLT_MON_STATE_UNDERVOLTAGE)), (UNITY_INT)((VoltMon_Ctx.state)), (
   ((void *)0)
   ), (UNITY_UINT)(84), UNITY_DISPLAY_STYLE_INT);
    UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT16)((0u)), (UNITY_INT)(UNITY_UINT16)((VoltMon_Ctx.uvActivationTimer_ms)), (
   ((void *)0)
   ), (UNITY_UINT)(85), UNITY_DISPLAY_STYLE_UINT16);
    UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT16)((0u)), (UNITY_INT)(UNITY_UINT16)((VoltMon_Ctx.ovActivationTimer_ms)), (
   ((void *)0)
   ), (UNITY_UINT)(86), UNITY_DISPLAY_STYLE_UINT16);
    UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT16)((0u)), (UNITY_INT)(UNITY_UINT16)((VoltMon_Ctx.deactivationTimer_ms)), (
   ((void *)0)
   ), (UNITY_UINT)(87), UNITY_DISPLAY_STYLE_UINT16);
}

void test_voltMonRun_NormalState_VoltageToOverVoltage(void)
{
    setUp();

    for(int i = 0; i < (VoltMon_ActivationTime_ms / 10u); i++)
{
        VoltMon_ReadVoltageProject_mV_CMockExpectAndReturn(101, (VoltMon_ThresholdOver_mV));
        VoltMon_GetUnderOn_mV_CMockExpectAndReturn(102, 8000u);
        VoltMon_GetUnderOff_mV_CMockExpectAndReturn(103, 8500u);
        VoltMon_GetOverOn_mV_CMockExpectAndReturn(104, 12500u);
        VoltMon_GetOverOff_mV_CMockExpectAndReturn(105, 13000u);

        voltMonRun(10u);
    }

    UnityAssertEqualNumber((UNITY_INT)((VOLT_MON_STATE_OVERVOLTAGE)), (UNITY_INT)((VoltMon_Ctx.state)), (
   ((void *)0)
   ), (UNITY_UINT)(111), UNITY_DISPLAY_STYLE_INT);
    UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT16)((0u)), (UNITY_INT)(UNITY_UINT16)((VoltMon_Ctx.uvActivationTimer_ms)), (
   ((void *)0)
   ), (UNITY_UINT)(112), UNITY_DISPLAY_STYLE_UINT16);
    UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT16)((0u)), (UNITY_INT)(UNITY_UINT16)((VoltMon_Ctx.ovActivationTimer_ms)), (
   ((void *)0)
   ), (UNITY_UINT)(113), UNITY_DISPLAY_STYLE_UINT16);
    UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT16)((0u)), (UNITY_INT)(UNITY_UINT16)((VoltMon_Ctx.deactivationTimer_ms)), (
   ((void *)0)
   ), (UNITY_UINT)(114), UNITY_DISPLAY_STYLE_UINT16);
}

void test_voltMonRun_UnderVoltageState_VoltageRecoveryToNormal(void)
{
    setUp();

    for(int i = 0; i < (VoltMon_ActivationTime_ms / 10u)+1; i++)
{
        VoltMon_ReadVoltageProject_mV_CMockExpectAndReturn(131, (VoltMon_ThresholdUnder_mV));
        VoltMon_GetUnderOn_mV_CMockExpectAndReturn(132, 8000u);
        VoltMon_GetUnderOff_mV_CMockExpectAndReturn(133, 8500u);
        VoltMon_GetOverOn_mV_CMockExpectAndReturn(134, 12500u);
        VoltMon_GetOverOff_mV_CMockExpectAndReturn(135, 13000u);

        voltMonRun(10u);
    }

    UnityAssertEqualNumber((UNITY_INT)((VOLT_MON_STATE_UNDERVOLTAGE)), (UNITY_INT)((VoltMon_Ctx.state)), (
   ((void *)0)
   ), (UNITY_UINT)(140), UNITY_DISPLAY_STYLE_INT);

    for(int i = 0; i < (VoltMon_DeactivationTime_ms / 10u)+1; i++)
{
        VoltMon_ReadVoltageProject_mV_CMockExpectAndReturn(145, (VoltMon_ThresholdUnder_mV+VoltMon_Hysteresis_mV));
        VoltMon_GetUnderOn_mV_CMockExpectAndReturn(146, 8000u);
        VoltMon_GetUnderOff_mV_CMockExpectAndReturn(147, 8500u);
        VoltMon_GetOverOn_mV_CMockExpectAndReturn(148, 12500u);
        VoltMon_GetOverOff_mV_CMockExpectAndReturn(149, 13000u);

        voltMonRun(10u);
    }

    UnityAssertEqualNumber((UNITY_INT)((VOLT_MON_STATE_NORMAL)), (UNITY_INT)((VoltMon_Ctx.state)), (
   ((void *)0)
   ), (UNITY_UINT)(155), UNITY_DISPLAY_STYLE_INT);
    UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT16)((0u)), (UNITY_INT)(UNITY_UINT16)((VoltMon_Ctx.deactivationTimer_ms)), (
   ((void *)0)
   ), (UNITY_UINT)(156), UNITY_DISPLAY_STYLE_UINT16);
}

void test_voltMonRun_UnderVoltageState_VoltageStaysUnderVoltage(void)
{
    setUp();

    for(int i = 0; i < (VoltMon_ActivationTime_ms / 10u)+1; i++)
{
        VoltMon_ReadVoltageProject_mV_CMockExpectAndReturn(169, (VoltMon_ThresholdUnder_mV));
        VoltMon_GetUnderOn_mV_CMockExpectAndReturn(170, 8000u);
        VoltMon_GetUnderOff_mV_CMockExpectAndReturn(171, 8500u);
        VoltMon_GetOverOn_mV_CMockExpectAndReturn(172, 12500u);
        VoltMon_GetOverOff_mV_CMockExpectAndReturn(173, 13000u);

        voltMonRun(10u);
    }

    UnityAssertEqualNumber((UNITY_INT)((VOLT_MON_STATE_UNDERVOLTAGE)), (UNITY_INT)((VoltMon_Ctx.state)), (
   ((void *)0)
   ), (UNITY_UINT)(178), UNITY_DISPLAY_STYLE_INT);

    for(int i = 0; i < 5; i++)
{
        VoltMon_ReadVoltageProject_mV_CMockExpectAndReturn(183, (VoltMon_ThresholdUnder_mV));
        VoltMon_GetUnderOn_mV_CMockExpectAndReturn(184, 8000u);
        VoltMon_GetUnderOff_mV_CMockExpectAndReturn(185, 8500u);
        VoltMon_GetOverOn_mV_CMockExpectAndReturn(186, 12500u);
        VoltMon_GetOverOff_mV_CMockExpectAndReturn(187, 13000u);

        voltMonRun(10u);
    }

    UnityAssertEqualNumber((UNITY_INT)((VOLT_MON_STATE_UNDERVOLTAGE)), (UNITY_INT)((VoltMon_Ctx.state)), (
   ((void *)0)
   ), (UNITY_UINT)(193), UNITY_DISPLAY_STYLE_INT);
    UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT16)((0u)), (UNITY_INT)(UNITY_UINT16)((VoltMon_Ctx.deactivationTimer_ms)), (
   ((void *)0)
   ), (UNITY_UINT)(194), UNITY_DISPLAY_STYLE_UINT16);
}

void test_voltMonRun_UnderVoltageState_PartialRecovery_ResetTimer(void)
{
    setUp();

    for(int i = 0; i < (VoltMon_ActivationTime_ms / 10u)+1; i++)
{
        VoltMon_ReadVoltageProject_mV_CMockExpectAndReturn(207, (VoltMon_ThresholdUnder_mV));
        VoltMon_GetUnderOn_mV_CMockExpectAndReturn(208, 8000u);
        VoltMon_GetUnderOff_mV_CMockExpectAndReturn(209, 8500u);
        VoltMon_GetOverOn_mV_CMockExpectAndReturn(210, 12500u);
        VoltMon_GetOverOff_mV_CMockExpectAndReturn(211, 13000u);

        voltMonRun(10u);
    }

    UnityAssertEqualNumber((UNITY_INT)((VOLT_MON_STATE_UNDERVOLTAGE)), (UNITY_INT)((VoltMon_Ctx.state)), (
   ((void *)0)
   ), (UNITY_UINT)(216), UNITY_DISPLAY_STYLE_INT);

    for(int i = 0; i < (VoltMon_DeactivationTime_ms / 10u)-1; i++)
{
        VoltMon_ReadVoltageProject_mV_CMockExpectAndReturn(221, (VoltMon_ThresholdUnder_mV+VoltMon_Hysteresis_mV));
        VoltMon_GetUnderOn_mV_CMockExpectAndReturn(222, 8000u);
        VoltMon_GetUnderOff_mV_CMockExpectAndReturn(223, 8500u);
        VoltMon_GetOverOn_mV_CMockExpectAndReturn(224, 12500u);
        VoltMon_GetOverOff_mV_CMockExpectAndReturn(225, 13000u);

        voltMonRun(10u);
    }

    VoltMon_ReadVoltageProject_mV_CMockExpectAndReturn(231, (VoltMon_ThresholdUnder_mV));
    VoltMon_GetUnderOn_mV_CMockExpectAndReturn(232, 8000u);
    VoltMon_GetUnderOff_mV_CMockExpectAndReturn(233, 8500u);
    VoltMon_GetOverOn_mV_CMockExpectAndReturn(234, 12500u);
    VoltMon_GetOverOff_mV_CMockExpectAndReturn(235, 13000u);

    voltMonRun(10u);

    UnityAssertEqualNumber((UNITY_INT)((VOLT_MON_STATE_UNDERVOLTAGE)), (UNITY_INT)((VoltMon_Ctx.state)), (
   ((void *)0)
   ), (UNITY_UINT)(240), UNITY_DISPLAY_STYLE_INT);
    UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT16)((0u)), (UNITY_INT)(UNITY_UINT16)((VoltMon_Ctx.deactivationTimer_ms)), (
   ((void *)0)
   ), (UNITY_UINT)(241), UNITY_DISPLAY_STYLE_UINT16);
}

void test_voltMonRun_OverVoltageState_VoltageRecoveryToNormal(void)
{
    setUp();

    for(int i = 0; i < (VoltMon_ActivationTime_ms / 10u); i++)
{
        VoltMon_ReadVoltageProject_mV_CMockExpectAndReturn(258, (VoltMon_ThresholdOver_mV));
        VoltMon_GetUnderOn_mV_CMockExpectAndReturn(259, 8000u);
        VoltMon_GetUnderOff_mV_CMockExpectAndReturn(260, 8500u);
        VoltMon_GetOverOn_mV_CMockExpectAndReturn(261, 12500u);
        VoltMon_GetOverOff_mV_CMockExpectAndReturn(262, 13000u);

        voltMonRun(10u);
    }

    UnityAssertEqualNumber((UNITY_INT)((VOLT_MON_STATE_OVERVOLTAGE)), (UNITY_INT)((VoltMon_Ctx.state)), (
   ((void *)0)
   ), (UNITY_UINT)(267), UNITY_DISPLAY_STYLE_INT);

    for(int i = 0; i < (VoltMon_DeactivationTime_ms / 10u)+1; i++)
{
        VoltMon_ReadVoltageProject_mV_CMockExpectAndReturn(272, (VoltMon_ThresholdOver_mV-VoltMon_Hysteresis_mV));
        VoltMon_GetUnderOn_mV_CMockExpectAndReturn(273, 8000u);
        VoltMon_GetUnderOff_mV_CMockExpectAndReturn(274, 8500u);
        VoltMon_GetOverOn_mV_CMockExpectAndReturn(275, 12500u);
        VoltMon_GetOverOff_mV_CMockExpectAndReturn(276, 13000u);

        voltMonRun(10u);
    }

    UnityAssertEqualNumber((UNITY_INT)((VOLT_MON_STATE_NORMAL)), (UNITY_INT)((VoltMon_Ctx.state)), (
   ((void *)0)
   ), (UNITY_UINT)(282), UNITY_DISPLAY_STYLE_INT);
    UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT16)((0u)), (UNITY_INT)(UNITY_UINT16)((VoltMon_Ctx.deactivationTimer_ms)), (
   ((void *)0)
   ), (UNITY_UINT)(283), UNITY_DISPLAY_STYLE_UINT16);
}

void test_voltMonRun_NormalState_UnderVoltageDebouncing_IncompleteTimer(void)
{
    setUp();

    for(int i = 0; i < (VoltMon_ActivationTime_ms / 10u)-1; i++)
{
        VoltMon_ReadVoltageProject_mV_CMockExpectAndReturn(300, (VoltMon_ThresholdUnder_mV));
        VoltMon_GetUnderOn_mV_CMockExpectAndReturn(301, 8000u);
        VoltMon_GetUnderOff_mV_CMockExpectAndReturn(302, 8500u);
        VoltMon_GetOverOn_mV_CMockExpectAndReturn(303, 12500u);
        VoltMon_GetOverOff_mV_CMockExpectAndReturn(304, 13000u);

        voltMonRun(10u);
    }

    UnityAssertEqualNumber((UNITY_INT)((VOLT_MON_STATE_NORMAL)), (UNITY_INT)((VoltMon_Ctx.state)), (
   ((void *)0)
   ), (UNITY_UINT)(310), UNITY_DISPLAY_STYLE_INT);
}

void test_voltMonRun_NormalState_OverVoltageDebouncing_IncompleteTimer(void)
{
    setUp();

    for(int i = 0; i < (VoltMon_ActivationTime_ms / 10u)-1; i++)
{
        VoltMon_ReadVoltageProject_mV_CMockExpectAndReturn(323, (VoltMon_ThresholdOver_mV));
        VoltMon_GetUnderOn_mV_CMockExpectAndReturn(324, 8000u);
        VoltMon_GetUnderOff_mV_CMockExpectAndReturn(325, 8500u);
        VoltMon_GetOverOn_mV_CMockExpectAndReturn(326, 12500u);
        VoltMon_GetOverOff_mV_CMockExpectAndReturn(327, 13000u);

        voltMonRun(10u);
    }

    UnityAssertEqualNumber((UNITY_INT)((VOLT_MON_STATE_NORMAL)), (UNITY_INT)((VoltMon_Ctx.state)), (
   ((void *)0)
   ), (UNITY_UINT)(333), UNITY_DISPLAY_STYLE_INT);
}

void test_voltMonRun_NormalState_VoltageOscillation_NoTransition(void)
{
    setUp();

    for(int i = 0; i < 3; i++)
{
        VoltMon_ReadVoltageProject_mV_CMockExpectAndReturn(346, (VoltMon_ThresholdUnder_mV));
        VoltMon_GetUnderOn_mV_CMockExpectAndReturn(347, 8000u);
        VoltMon_GetUnderOff_mV_CMockExpectAndReturn(348, 8500u);
        VoltMon_GetOverOn_mV_CMockExpectAndReturn(349, 12500u);
        VoltMon_GetOverOff_mV_CMockExpectAndReturn(350, 13000u);
        voltMonRun(10u);

        VoltMon_ReadVoltageProject_mV_CMockExpectAndReturn(353, 10000u);
        VoltMon_GetUnderOn_mV_CMockExpectAndReturn(354, 8000u);
        VoltMon_GetUnderOff_mV_CMockExpectAndReturn(355, 8500u);
        VoltMon_GetOverOn_mV_CMockExpectAndReturn(356, 12500u);
        VoltMon_GetOverOff_mV_CMockExpectAndReturn(357, 13000u);
        voltMonRun(10u);
    }

    UnityAssertEqualNumber((UNITY_INT)((VOLT_MON_STATE_NORMAL)), (UNITY_INT)((VoltMon_Ctx.state)), (
   ((void *)0)
   ), (UNITY_UINT)(362), UNITY_DISPLAY_STYLE_INT);
    UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT16)((0u)), (UNITY_INT)(UNITY_UINT16)((VoltMon_Ctx.uvActivationTimer_ms)), (
   ((void *)0)
   ), (UNITY_UINT)(363), UNITY_DISPLAY_STYLE_UINT16);
    UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT16)((0u)), (UNITY_INT)(UNITY_UINT16)((VoltMon_Ctx.ovActivationTimer_ms)), (
   ((void *)0)
   ), (UNITY_UINT)(364), UNITY_DISPLAY_STYLE_UINT16);
}

void test_voltMonRun_Hysteresis_UnderVoltage_OffThreshold(void)
{
    setUp();

    for(int i = 0; i < (VoltMon_ActivationTime_ms / 10u)+1; i++)
{
        VoltMon_ReadVoltageProject_mV_CMockExpectAndReturn(381, (VoltMon_ThresholdUnder_mV));
        VoltMon_GetUnderOn_mV_CMockExpectAndReturn(382, 8000u);
        VoltMon_GetUnderOff_mV_CMockExpectAndReturn(383, 8500u);
        VoltMon_GetOverOn_mV_CMockExpectAndReturn(384, 12500u);
        VoltMon_GetOverOff_mV_CMockExpectAndReturn(385, 13000u);

        voltMonRun(10u);
    }

    UnityAssertEqualNumber((UNITY_INT)((VOLT_MON_STATE_UNDERVOLTAGE)), (UNITY_INT)((VoltMon_Ctx.state)), (
   ((void *)0)
   ), (UNITY_UINT)(390), UNITY_DISPLAY_STYLE_INT);

    for(int i = 0; i < (VoltMon_DeactivationTime_ms / 10u)+1; i++)
{
        VoltMon_ReadVoltageProject_mV_CMockExpectAndReturn(395, 8500u);
        VoltMon_GetUnderOn_mV_CMockExpectAndReturn(396, 8000u);
        VoltMon_GetUnderOff_mV_CMockExpectAndReturn(397, 8500u);
        VoltMon_GetOverOn_mV_CMockExpectAndReturn(398, 12500u);
        VoltMon_GetOverOff_mV_CMockExpectAndReturn(399, 13000u);

        voltMonRun(10u);
    }

    UnityAssertEqualNumber((UNITY_INT)((VOLT_MON_STATE_NORMAL)), (UNITY_INT)((VoltMon_Ctx.state)), (
   ((void *)0)
   ), (UNITY_UINT)(405), UNITY_DISPLAY_STYLE_INT);
}

void test_voltMonRun_Hysteresis_OverVoltage_OffThreshold(void)
{
    setUp();

    for(int i = 0; i < (VoltMon_ActivationTime_ms / 10u); i++)
{
        VoltMon_ReadVoltageProject_mV_CMockExpectAndReturn(418, (VoltMon_ThresholdOver_mV));
        VoltMon_GetUnderOn_mV_CMockExpectAndReturn(419, 8000u);
        VoltMon_GetUnderOff_mV_CMockExpectAndReturn(420, 8500u);
        VoltMon_GetOverOn_mV_CMockExpectAndReturn(421, 12500u);
        VoltMon_GetOverOff_mV_CMockExpectAndReturn(422, 13000u);

        voltMonRun(10u);
    }

    UnityAssertEqualNumber((UNITY_INT)((VOLT_MON_STATE_OVERVOLTAGE)), (UNITY_INT)((VoltMon_Ctx.state)), (
   ((void *)0)
   ), (UNITY_UINT)(427), UNITY_DISPLAY_STYLE_INT);

    for(int i = 0; i < (VoltMon_DeactivationTime_ms / 10u)+1; i++)
{
        VoltMon_ReadVoltageProject_mV_CMockExpectAndReturn(432, 13000u);
        VoltMon_GetUnderOn_mV_CMockExpectAndReturn(433, 8000u);
        VoltMon_GetUnderOff_mV_CMockExpectAndReturn(434, 8500u);
        VoltMon_GetOverOn_mV_CMockExpectAndReturn(435, 12500u);
        VoltMon_GetOverOff_mV_CMockExpectAndReturn(436, 13000u);

        voltMonRun(10u);
    }

    UnityAssertEqualNumber((UNITY_INT)((VOLT_MON_STATE_NORMAL)), (UNITY_INT)((VoltMon_Ctx.state)), (
   ((void *)0)
   ), (UNITY_UINT)(442), UNITY_DISPLAY_STYLE_INT);
}

void test_voltMonRun_CyclicTransitions_NORMAL_UV_NORMAL(void)
{
    setUp();
    UnityAssertEqualNumber((UNITY_INT)((VOLT_MON_STATE_NORMAL)), (UNITY_INT)((VoltMon_Ctx.state)), (
   ((void *)0)
   ), (UNITY_UINT)(455), UNITY_DISPLAY_STYLE_INT);

    for(int i = 0; i < (VoltMon_ActivationTime_ms / 10u)+1; i++)
{
        VoltMon_ReadVoltageProject_mV_CMockExpectAndReturn(460, 7500u);
        VoltMon_GetUnderOn_mV_CMockExpectAndReturn(461, 8000u);
        VoltMon_GetUnderOff_mV_CMockExpectAndReturn(462, 8500u);
        VoltMon_GetOverOn_mV_CMockExpectAndReturn(463, 12500u);
        VoltMon_GetOverOff_mV_CMockExpectAndReturn(464, 13000u);

        voltMonRun(10u);
    }

    UnityAssertEqualNumber((UNITY_INT)((VOLT_MON_STATE_UNDERVOLTAGE)), (UNITY_INT)((VoltMon_Ctx.state)), (
   ((void *)0)
   ), (UNITY_UINT)(469), UNITY_DISPLAY_STYLE_INT);

    for(int i = 0; i < (VoltMon_DeactivationTime_ms / 10u)+1; i++)
{
        VoltMon_ReadVoltageProject_mV_CMockExpectAndReturn(474, 9000u);
        VoltMon_GetUnderOn_mV_CMockExpectAndReturn(475, 8000u);
        VoltMon_GetUnderOff_mV_CMockExpectAndReturn(476, 8500u);
        VoltMon_GetOverOn_mV_CMockExpectAndReturn(477, 12500u);
        VoltMon_GetOverOff_mV_CMockExpectAndReturn(478, 13000u);

        voltMonRun(10u);
    }

    UnityAssertEqualNumber((UNITY_INT)((VOLT_MON_STATE_NORMAL)), (UNITY_INT)((VoltMon_Ctx.state)), (
   ((void *)0)
   ), (UNITY_UINT)(484), UNITY_DISPLAY_STYLE_INT);
    UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT16)((0u)), (UNITY_INT)(UNITY_UINT16)((VoltMon_Ctx.uvActivationTimer_ms)), (
   ((void *)0)
   ), (UNITY_UINT)(485), UNITY_DISPLAY_STYLE_UINT16);
    UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT16)((0u)), (UNITY_INT)(UNITY_UINT16)((VoltMon_Ctx.ovActivationTimer_ms)), (
   ((void *)0)
   ), (UNITY_UINT)(486), UNITY_DISPLAY_STYLE_UINT16);
    UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT16)((0u)), (UNITY_INT)(UNITY_UINT16)((VoltMon_Ctx.deactivationTimer_ms)), (
   ((void *)0)
   ), (UNITY_UINT)(487), UNITY_DISPLAY_STYLE_UINT16);
}

void test_voltMonRun_CyclicTransitions_NORMAL_OV_NORMAL(void)
{
    setUp();
    UnityAssertEqualNumber((UNITY_INT)((VOLT_MON_STATE_NORMAL)), (UNITY_INT)((VoltMon_Ctx.state)), (
   ((void *)0)
   ), (UNITY_UINT)(496), UNITY_DISPLAY_STYLE_INT);

    for(int i = 0; i < (VoltMon_ActivationTime_ms / 10u); i++)
{
        VoltMon_ReadVoltageProject_mV_CMockExpectAndReturn(501, 13500u);
        VoltMon_GetUnderOn_mV_CMockExpectAndReturn(502, 8000u);
        VoltMon_GetUnderOff_mV_CMockExpectAndReturn(503, 8500u);
        VoltMon_GetOverOn_mV_CMockExpectAndReturn(504, 12500u);
        VoltMon_GetOverOff_mV_CMockExpectAndReturn(505, 13000u);

        voltMonRun(10u);
    }

    UnityAssertEqualNumber((UNITY_INT)((VOLT_MON_STATE_OVERVOLTAGE)), (UNITY_INT)((VoltMon_Ctx.state)), (
   ((void *)0)
   ), (UNITY_UINT)(510), UNITY_DISPLAY_STYLE_INT);

    for(int i = 0; i < (VoltMon_DeactivationTime_ms / 10u)+1; i++)
{
        VoltMon_ReadVoltageProject_mV_CMockExpectAndReturn(515, 12000u);
        VoltMon_GetUnderOn_mV_CMockExpectAndReturn(516, 8000u);
        VoltMon_GetUnderOff_mV_CMockExpectAndReturn(517, 8500u);
        VoltMon_GetOverOn_mV_CMockExpectAndReturn(518, 12500u);
        VoltMon_GetOverOff_mV_CMockExpectAndReturn(519, 13000u);

        voltMonRun(10u);
    }

    UnityAssertEqualNumber((UNITY_INT)((VOLT_MON_STATE_NORMAL)), (UNITY_INT)((VoltMon_Ctx.state)), (
   ((void *)0)
   ), (UNITY_UINT)(525), UNITY_DISPLAY_STYLE_INT);
    UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT16)((0u)), (UNITY_INT)(UNITY_UINT16)((VoltMon_Ctx.uvActivationTimer_ms)), (
   ((void *)0)
   ), (UNITY_UINT)(526), UNITY_DISPLAY_STYLE_UINT16);
    UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT16)((0u)), (UNITY_INT)(UNITY_UINT16)((VoltMon_Ctx.ovActivationTimer_ms)), (
   ((void *)0)
   ), (UNITY_UINT)(527), UNITY_DISPLAY_STYLE_UINT16);
    UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT16)((0u)), (UNITY_INT)(UNITY_UINT16)((VoltMon_Ctx.deactivationTimer_ms)), (
   ((void *)0)
   ), (UNITY_UINT)(528), UNITY_DISPLAY_STYLE_UINT16);
}

void test_voltMonRun_LongDurationUnderVoltage(void)
{
    setUp();

    for(int i = 0; i < (VoltMon_ActivationTime_ms / 10u)+5; i++)
{
        VoltMon_ReadVoltageProject_mV_CMockExpectAndReturn(541, 7000u);
        VoltMon_GetUnderOn_mV_CMockExpectAndReturn(542, 8000u);
        VoltMon_GetUnderOff_mV_CMockExpectAndReturn(543, 8500u);
        VoltMon_GetOverOn_mV_CMockExpectAndReturn(544, 12500u);
        VoltMon_GetOverOff_mV_CMockExpectAndReturn(545, 13000u);

        voltMonRun(10u);
    }

    UnityAssertEqualNumber((UNITY_INT)((VOLT_MON_STATE_UNDERVOLTAGE)), (UNITY_INT)((VoltMon_Ctx.state)), (
   ((void *)0)
   ), (UNITY_UINT)(551), UNITY_DISPLAY_STYLE_INT);
    UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT16)((0u)), (UNITY_INT)(UNITY_UINT16)((VoltMon_Ctx.deactivationTimer_ms)), (
   ((void *)0)
   ), (UNITY_UINT)(552), UNITY_DISPLAY_STYLE_UINT16);
}

void test_voltMonRun_LongDurationOverVoltage(void)
{
    setUp();

    for(int i = 0; i < (VoltMon_ActivationTime_ms / 10u)+5; i++)
{
        VoltMon_ReadVoltageProject_mV_CMockExpectAndReturn(565, 14000u);
        VoltMon_GetUnderOn_mV_CMockExpectAndReturn(566, 8000u);
        VoltMon_GetUnderOff_mV_CMockExpectAndReturn(567, 8500u);
        VoltMon_GetOverOn_mV_CMockExpectAndReturn(568, 12500u);
        VoltMon_GetOverOff_mV_CMockExpectAndReturn(569, 13000u);

        voltMonRun(10u);
    }

    UnityAssertEqualNumber((UNITY_INT)((VOLT_MON_STATE_OVERVOLTAGE)), (UNITY_INT)((VoltMon_Ctx.state)), (
   ((void *)0)
   ), (UNITY_UINT)(575), UNITY_DISPLAY_STYLE_INT);
    UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT16)((0u)), (UNITY_INT)(UNITY_UINT16)((VoltMon_Ctx.deactivationTimer_ms)), (
   ((void *)0)
   ), (UNITY_UINT)(576), UNITY_DISPLAY_STYLE_UINT16);
}

void test_voltMonRun_BoundaryVoltage_JustBelowUnderOn(void)
{
    setUp();

    for(int i = 0; i < 2; i++)
{
        VoltMon_ReadVoltageProject_mV_CMockExpectAndReturn(589, 7999u);
        VoltMon_GetUnderOn_mV_CMockExpectAndReturn(590, 8000u);
        VoltMon_GetUnderOff_mV_CMockExpectAndReturn(591, 8500u);
        VoltMon_GetOverOn_mV_CMockExpectAndReturn(592, 12500u);
        VoltMon_GetOverOff_mV_CMockExpectAndReturn(593, 13000u);

        voltMonRun(10u);
    }

    UnityAssertEqualNumber((UNITY_INT)((VOLT_MON_STATE_NORMAL)), (UNITY_INT)((VoltMon_Ctx.state)), (
   ((void *)0)
   ), (UNITY_UINT)(599), UNITY_DISPLAY_STYLE_INT);
}

void test_voltMonRun_BoundaryVoltage_JustAboveOverOn(void)
{
    setUp();

    for(int i = 0; i < 2; i++)
{
        VoltMon_ReadVoltageProject_mV_CMockExpectAndReturn(612, 12501u);
        VoltMon_GetUnderOn_mV_CMockExpectAndReturn(613, 8000u);
        VoltMon_GetUnderOff_mV_CMockExpectAndReturn(614, 8500u);
        VoltMon_GetOverOn_mV_CMockExpectAndReturn(615, 12500u);
        VoltMon_GetOverOff_mV_CMockExpectAndReturn(616, 13000u);

        voltMonRun(10u);
    }

    UnityAssertEqualNumber((UNITY_INT)((VOLT_MON_STATE_NORMAL)), (UNITY_INT)((VoltMon_Ctx.state)), (
   ((void *)0)
   ), (UNITY_UINT)(622), UNITY_DISPLAY_STYLE_INT);
}

void test_voltMonRun_ZeroVoltage(void)
{
    setUp();

    for(int i = 0; i < (VoltMon_ActivationTime_ms / 10u)+1; i++)
{
        VoltMon_ReadVoltageProject_mV_CMockExpectAndReturn(635, 0u);
        VoltMon_GetUnderOn_mV_CMockExpectAndReturn(636, 8000u);
        VoltMon_GetUnderOff_mV_CMockExpectAndReturn(637, 8500u);
        VoltMon_GetOverOn_mV_CMockExpectAndReturn(638, 12500u);
        VoltMon_GetOverOff_mV_CMockExpectAndReturn(639, 13000u);

        voltMonRun(10u);
    }

    UnityAssertEqualNumber((UNITY_INT)((VOLT_MON_STATE_UNDERVOLTAGE)), (UNITY_INT)((VoltMon_Ctx.state)), (
   ((void *)0)
   ), (UNITY_UINT)(645), UNITY_DISPLAY_STYLE_INT);
}

void test_voltMonRun_VeryHighVoltage(void)
{
    setUp();

    for(int i = 0; i < (VoltMon_ActivationTime_ms / 10u); i++)
{
        VoltMon_ReadVoltageProject_mV_CMockExpectAndReturn(658, 65535u);
        VoltMon_GetUnderOn_mV_CMockExpectAndReturn(659, 8000u);
        VoltMon_GetUnderOff_mV_CMockExpectAndReturn(660, 8500u);
        VoltMon_GetOverOn_mV_CMockExpectAndReturn(661, 12500u);
        VoltMon_GetOverOff_mV_CMockExpectAndReturn(662, 13000u);

        voltMonRun(10u);
    }

    UnityAssertEqualNumber((UNITY_INT)((VOLT_MON_STATE_OVERVOLTAGE)), (UNITY_INT)((VoltMon_Ctx.state)), (
   ((void *)0)
   ), (UNITY_UINT)(668), UNITY_DISPLAY_STYLE_INT);
}

void test_voltMonRun_TimerOverflow_LongElapsedTime(void)
{
    setUp();

    VoltMon_ReadVoltageProject_mV_CMockExpectAndReturn(679, (VoltMon_ThresholdUnder_mV));
    VoltMon_GetUnderOn_mV_CMockExpectAndReturn(680, 8000u);
    VoltMon_GetUnderOff_mV_CMockExpectAndReturn(681, 8500u);
    VoltMon_GetOverOn_mV_CMockExpectAndReturn(682, 12500u);
    VoltMon_GetOverOff_mV_CMockExpectAndReturn(683, 13000u);

    voltMonRun(1000u);

    UnityAssertEqualNumber((UNITY_INT)((VOLT_MON_STATE_UNDERVOLTAGE)), (UNITY_INT)((VoltMon_Ctx.state)), (
   ((void *)0)
   ), (UNITY_UINT)(688), UNITY_DISPLAY_STYLE_INT);
}

void test_voltMonRun_AlternatingStates_NORMAL_UV_OV_NORMAL(void)
{
    setUp();

    for(int i = 0; i < (VoltMon_ActivationTime_ms / 10u)+1; i++)
{
        VoltMon_ReadVoltageProject_mV_CMockExpectAndReturn(701, 7500u);
        VoltMon_GetUnderOn_mV_CMockExpectAndReturn(702, 8000u);
        VoltMon_GetUnderOff_mV_CMockExpectAndReturn(703, 8500u);
        VoltMon_GetOverOn_mV_CMockExpectAndReturn(704, 12500u);
        VoltMon_GetOverOff_mV_CMockExpectAndReturn(705, 13000u);
        voltMonRun(10u);
    }
    UnityAssertEqualNumber((UNITY_INT)((VOLT_MON_STATE_UNDERVOLTAGE)), (UNITY_INT)((VoltMon_Ctx.state)), (
   ((void *)0)
   ), (UNITY_UINT)(708), UNITY_DISPLAY_STYLE_INT);

    for(int i = 0; i < (VoltMon_DeactivationTime_ms / 10u)+1; i++)
{
        VoltMon_ReadVoltageProject_mV_CMockExpectAndReturn(713, 10000u);
        VoltMon_GetUnderOn_mV_CMockExpectAndReturn(714, 8000u);
        VoltMon_GetUnderOff_mV_CMockExpectAndReturn(715, 8500u);
        VoltMon_GetOverOn_mV_CMockExpectAndReturn(716, 12500u);
        VoltMon_GetOverOff_mV_CMockExpectAndReturn(717, 13000u);
        voltMonRun(10u);
    }
    UnityAssertEqualNumber((UNITY_INT)((VOLT_MON_STATE_NORMAL)), (UNITY_INT)((VoltMon_Ctx.state)), (
   ((void *)0)
   ), (UNITY_UINT)(720), UNITY_DISPLAY_STYLE_INT);

    for(int i = 0; i < (VoltMon_ActivationTime_ms / 10u); i++)
{
        VoltMon_ReadVoltageProject_mV_CMockExpectAndReturn(725, 13500u);
        VoltMon_GetUnderOn_mV_CMockExpectAndReturn(726, 8000u);
        VoltMon_GetUnderOff_mV_CMockExpectAndReturn(727, 8500u);
        VoltMon_GetOverOn_mV_CMockExpectAndReturn(728, 12500u);
        VoltMon_GetOverOff_mV_CMockExpectAndReturn(729, 13000u);
        voltMonRun(10u);
    }
    UnityAssertEqualNumber((UNITY_INT)((VOLT_MON_STATE_OVERVOLTAGE)), (UNITY_INT)((VoltMon_Ctx.state)), (
   ((void *)0)
   ), (UNITY_UINT)(732), UNITY_DISPLAY_STYLE_INT);

    for(int i = 0; i < (VoltMon_DeactivationTime_ms / 10u)+1; i++)
{
        VoltMon_ReadVoltageProject_mV_CMockExpectAndReturn(737, 10000u);
        VoltMon_GetUnderOn_mV_CMockExpectAndReturn(738, 8000u);
        VoltMon_GetUnderOff_mV_CMockExpectAndReturn(739, 8500u);
        VoltMon_GetOverOn_mV_CMockExpectAndReturn(740, 12500u);
        VoltMon_GetOverOff_mV_CMockExpectAndReturn(741, 13000u);
        voltMonRun(10u);
    }

    UnityAssertEqualNumber((UNITY_INT)((VOLT_MON_STATE_NORMAL)), (UNITY_INT)((VoltMon_Ctx.state)), (
   ((void *)0)
   ), (UNITY_UINT)(746), UNITY_DISPLAY_STYLE_INT);
    UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT16)((0u)), (UNITY_INT)(UNITY_UINT16)((VoltMon_Ctx.uvActivationTimer_ms)), (
   ((void *)0)
   ), (UNITY_UINT)(747), UNITY_DISPLAY_STYLE_UINT16);
    UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT16)((0u)), (UNITY_INT)(UNITY_UINT16)((VoltMon_Ctx.ovActivationTimer_ms)), (
   ((void *)0)
   ), (UNITY_UINT)(748), UNITY_DISPLAY_STYLE_UINT16);
    UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT16)((0u)), (UNITY_INT)(UNITY_UINT16)((VoltMon_Ctx.deactivationTimer_ms)), (
   ((void *)0)
   ), (UNITY_UINT)(749), UNITY_DISPLAY_STYLE_UINT16);
}