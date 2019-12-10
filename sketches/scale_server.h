#pragma once

#define GPIO14			14							/* ������ clock HX711 */
#define GPIO16			16							/* ������ ���� HX711 */
#define EN_NCP			12							/* ������ ��������� �������  */
#define PWR_SW			13							/* ������ �� ������ ������� */
#define SW_CL			15							/* */
#define LED				2							/* ��������� ������ */
#define SCL_RTC			5							/* ������ clock ����� */
#define SDA_RTC			4							/* ������ ���� ����� */
#define V_BAT			A0							/* ��� ��� ������� */

#define R1_KOM					300.0f
#define R2_KOM					100.0f
#define VREF					1.0f
#define ADC						1023.0f

#define V_BAT_MAX				4.3f
#define V_BAT_MIN				3.5f

#define LED_ON					LOW
#define LED_OFF					HIGH