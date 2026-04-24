#pragma once

#include "../r2sdk/interfaces/CMatSystemSurface.h"

class Renderer {
public:
	static bool IsReady() {
		return MatSystemSurface != nullptr;
	}

	static bool GetScreenSize(int& width, int& height) {
		if (!MatSystemSurface) {
			width = 0;
			height = 0;
			return false;
		}

		MatSystemSurface->GetScreenSize(&width, &height);
		return true;
	}

	static void DrawFilledRect(int x, int y, int w, int h, const Color& color) {
		if (!MatSystemSurface) {
			return;
		}

		MatSystemSurface->DrawSetColorRGBA(
			static_cast<char>(color.r),
			static_cast<char>(color.g),
			static_cast<char>(color.b),
			color.a);
		MatSystemSurface->DrawFilledRect(x, y, x + w, y + h);
	}

	static void DrawOutlinedRect(int x, int y, int w, int h, const Color& color) {
		if (!MatSystemSurface) {
			return;
		}

		MatSystemSurface->DrawSetColorRGBA(
			static_cast<char>(color.r),
			static_cast<char>(color.g),
			static_cast<char>(color.b),
			color.a);
		MatSystemSurface->DrawOutlinedRect(x, y, x + w, y + h);
	}

	static void DrawLine(int x1, int y1, int x2, int y2, const Color& color) {
		if (!MatSystemSurface) {
			return;
		}

		MatSystemSurface->DrawSetColorRGBA(
			static_cast<char>(color.r),
			static_cast<char>(color.g),
			static_cast<char>(color.b),
			color.a);
		MatSystemSurface->DrawLine(x1, y1, x2, y2);
	}

	static void RDrawText(int font, int x, int y, const Color& color, const char* text) {
		if (!MatSystemSurface || !text) {
			return;
		}

		MatSystemSurface->DrawColoredTextWrapper(
			font,
			x,
			y,
			color.r,
			color.g,
			color.b,
			color.a,
			text);
	}

	static uintptr_t BeginPanelDraw() {
		if (!MatSystemSurface) {
			return 0;
		}

		const uintptr_t panel = MatSystemSurface->GetEmbeddedPanel();
		if (panel) {
			MatSystemSurface->PushMakeCurrent(panel, true);
		}
		return panel;
	}

	static void EndPanelDraw(uintptr_t panel) {
		if (!MatSystemSurface || !panel) {
			return;
		}

		MatSystemSurface->PopMakeCurrent(panel);
	}

	static bool DrawSelfTestOverlay() {
		if (!MatSystemSurface) {
			return false;
		}

		int w = 0;
		int h = 0;
		if (!GetScreenSize(w, h) || w <= 0 || h <= 0) {
			return false;
		}

		const uintptr_t panel = BeginPanelDraw();

		const int cx = w / 2;
		const int cy = h / 2;

		DrawOutlinedRect(cx - 120, cy - 70, 240, 140, Color(0, 255, 0, 255));
		DrawLine(cx - 15, cy, cx + 15, cy, Color(255, 0, 0, 255));
		DrawLine(cx, cy - 15, cx, cy + 15, Color(255, 0, 0, 255));
		RDrawText(5, cx - 100, cy - 90, Color(0, 255, 0, 255), "RENDERER SELF TEST");

		EndPanelDraw(panel);
		return true;
	}
};
