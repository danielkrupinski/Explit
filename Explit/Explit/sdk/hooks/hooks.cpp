#include "hooks.hpp"
c_hooks g_hooks;

#pragma region zgui
void line(int x, int y, int x2, int y2, zgui::color c) noexcept { g_draw.line(x, y, x2, y2, c); }
void rect(int x, int y, int x2, int y2, zgui::color c) noexcept { g_draw.rect(x, y, x2, y2, c); }
void filled_rect(int x, int y, int x2, int y2, zgui::color c) noexcept { g_draw.fill_rect(x, y, x2, y2, c); }
void text(int x, int y, zgui::color color, int font, bool center, const char* text) noexcept { g_draw.string(x, y, color, font, center, text); }
void get_text_size(unsigned long font, const char* text, int& wide, int& tall) noexcept { g_draw.get_text_size(font, text, wide, tall); }
float get_frametime() noexcept { return g_interfaces.p_globalvars->frame_time; }
#pragma endregion


void c_hooks::get_hooks() 
{
	panel_hook = std::make_unique<vmt>(g_interfaces.p_panel);
	surface_hook = std::make_unique<vmt>(g_interfaces.p_surface);
	panel_hook->hook(41, painttraverse);
	//surface_hook->hook(67, LockCursor);
	zgui::functions.draw_line = line;
	zgui::functions.draw_rect = rect;
	zgui::functions.draw_filled_rect = filled_rect;
	zgui::functions.draw_text = text;
	zgui::functions.get_text_size = get_text_size;
	zgui::functions.get_frametime = get_frametime;
}
void c_hooks::un_hooks()
{
	panel_hook->unhook(41);
	//surface_hook->unhook(67);
}
void __fastcall c_hooks::painttraverse(PVOID pPanels, int edx, unsigned int vguiPanel, bool forceRepaint, bool allowForce)
{
	static auto ohook = g_hooks.panel_hook->getorginal<painttraverse_t>(41);
	static uint32_t overlaypanel;
	if (!overlaypanel)
	{
		const char* panel_name = g_interfaces.p_panel->get_name(vguiPanel);

		if (lstrcmpA(panel_name, "MatSystemTopPanel") == 0)
		{
			overlaypanel = vguiPanel;
			g_draw.menu = g_interfaces.p_surface->createfont_();
			g_interfaces.p_surface->setfontglyphset(g_draw.menu, "Tahoma", 12, 540, 0, 0, fontflag_outline | fontflag_dropshadow);
			g_draw.esp = g_interfaces.p_surface->createfont_();
			g_interfaces.p_surface->setfontglyphset(g_draw.esp, "Tahoma", 11, 500, 0, 0, fontflag_outline );
		}
	}
	if (overlaypanel == vguiPanel)
	{
		g_menu.draw();
		g_interfaces.g_localplayer = static_cast<c_base_entity*>(g_interfaces.p_entitylist->get_client_entity((g_interfaces.p_engine->get_local_player())));
		if (g_interfaces.g_localplayer && g_interfaces.p_engine->is_in_game() && g_interfaces.p_engine->is_connected())
			g_esp.start();
		//g_draw.fill_rect(20, 20, 20, 20, Color(255, 255, 255, 255));
	}
	ohook(pPanels, vguiPanel, forceRepaint, allowForce);
}

void __fastcall c_hooks::lockcursor(i_surface* thisptr, void* edx)
{
	static auto ohook = g_hooks.panel_hook->getorginal<lockcursor_t>(67);

	if (!zgui::open)
		return ohook(thisptr, edx);

	g_interfaces.p_surface->unlockcursor();
}
