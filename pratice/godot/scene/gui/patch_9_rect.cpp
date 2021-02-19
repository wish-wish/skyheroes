/*************************************************************************/
/*  patch_9_rect.cpp                                                     */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                    http://www.godotengine.org                         */
/*************************************************************************/
/* Copyright (c) 2007-2017 Juan Linietsky, Ariel Manzur.                 */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/
#include "patch_9_rect.h"

#include "servers/visual_server.h"

void NinePatchRect::_notification(int p_what) {

	if (p_what==NOTIFICATION_DRAW) {

		if (texture.is_null())
			return;

		Size2 s=get_size();
		RID ci = get_canvas_item();
		VS::get_singleton()->canvas_item_add_nine_patch(ci,Rect2(Point2(),s),region_rect,texture->get_rid(),Vector2(margin[MARGIN_LEFT],margin[MARGIN_TOP]),Vector2(margin[MARGIN_RIGHT],margin[MARGIN_BOTTOM]),VS::NINE_PATCH_STRETCH,VS::NINE_PATCH_STRETCH,draw_center);
		//draw_texture_rect(texture,Rect2(Point2(),s),false,modulate);

/*
		Vector<Point2> points;
		points.resize(4);
		points[0]=Point2(0,0);
		points[1]=Point2(s.x,0);
		points[2]=Point2(s.x,s.y);
		points[3]=Point2(0,s.y);
		Vector<Point2> uvs;
		uvs.resize(4);
		uvs[0]=Point2(0,0);
		uvs[1]=Point2(1,0);
		uvs[2]=Point2(1,1);
		uvs[3]=Point2(0,1);

		VisualServer::get_singleton()->canvas_item_add_primitive(ci,points,Vector<Color>(),uvs,texture->get_rid());
*/
	}
}

Size2 NinePatchRect::get_minimum_size() const {

	return Size2(margin[MARGIN_LEFT]+margin[MARGIN_RIGHT],margin[MARGIN_TOP]+margin[MARGIN_BOTTOM]);
}
void NinePatchRect::_bind_methods() {


	ClassDB::bind_method(_MD("set_texture","texture"), & NinePatchRect::set_texture );
	ClassDB::bind_method(_MD("get_texture"), & NinePatchRect::get_texture );
	ClassDB::bind_method(_MD("set_patch_margin","margin","value"), & NinePatchRect::set_patch_margin );
	ClassDB::bind_method(_MD("get_patch_margin","margin"), & NinePatchRect::get_patch_margin );
	ClassDB::bind_method(_MD("set_region_rect","rect"),&NinePatchRect::set_region_rect);
	ClassDB::bind_method(_MD("get_region_rect"),&NinePatchRect::get_region_rect);
	ClassDB::bind_method(_MD("set_draw_center","draw_center"), & NinePatchRect::set_draw_center );
	ClassDB::bind_method(_MD("get_draw_center"), & NinePatchRect::get_draw_center );

	ADD_SIGNAL(MethodInfo("texture_changed"));

	ADD_PROPERTYNZ( PropertyInfo( Variant::OBJECT, "texture", PROPERTY_HINT_RESOURCE_TYPE, "Texture"), _SCS("set_texture"),_SCS("get_texture") );
	ADD_PROPERTYNO( PropertyInfo( Variant::BOOL, "draw_center"), _SCS("set_draw_center"),_SCS("get_draw_center") );
	ADD_PROPERTYNZ( PropertyInfo( Variant::RECT2, "region_rect"), _SCS("set_region_rect"),_SCS("get_region_rect"));

	ADD_GROUP("Patch Margin","patch_margin_");
	ADD_PROPERTYINZ( PropertyInfo( Variant::INT, "patch_margin_left",PROPERTY_HINT_RANGE,"0,16384,1"), _SCS("set_patch_margin"),_SCS("get_patch_margin"),MARGIN_LEFT );
	ADD_PROPERTYINZ( PropertyInfo( Variant::INT, "patch_margin_top",PROPERTY_HINT_RANGE,"0,16384,1"), _SCS("set_patch_margin"),_SCS("get_patch_margin"),MARGIN_TOP );
	ADD_PROPERTYINZ( PropertyInfo( Variant::INT, "patch_margin_right",PROPERTY_HINT_RANGE,"0,16384,1"), _SCS("set_patch_margin"),_SCS("get_patch_margin"),MARGIN_RIGHT );
	ADD_PROPERTYINZ( PropertyInfo( Variant::INT, "patch_margin_bottom",PROPERTY_HINT_RANGE,"0,16384,1"), _SCS("set_patch_margin"),_SCS("get_patch_margin"),MARGIN_BOTTOM );

}


void NinePatchRect::set_texture(const Ref<Texture>& p_tex) {

	if (texture==p_tex)
		return;
	texture=p_tex;
	update();
	/*
	if (texture.is_valid())
		texture->set_flags(texture->get_flags()&(~Texture::FLAG_REPEAT)); //remove repeat from texture, it looks bad in sprites
	*/
	minimum_size_changed();
	emit_signal("texture_changed");
}

Ref<Texture> NinePatchRect::get_texture() const {

	return texture;
}



void NinePatchRect::set_patch_margin(Margin p_margin,int p_size) {

	ERR_FAIL_INDEX(p_margin,4);
	margin[p_margin]=p_size;
	update();
	minimum_size_changed();
	switch (p_margin) {
		case MARGIN_LEFT:
			_change_notify("patch_margin/left");
			break;
		case MARGIN_TOP:
			_change_notify("patch_margin/top");
			break;
		case MARGIN_RIGHT:
			_change_notify("patch_margin/right");
			break;
		case MARGIN_BOTTOM:
			_change_notify("patch_margin/bottom");
			break;
	}
}

int NinePatchRect::get_patch_margin(Margin p_margin) const{

	ERR_FAIL_INDEX_V(p_margin,4,0);
	return margin[p_margin];
}

void NinePatchRect::set_region_rect(const Rect2& p_region_rect) {

	if (region_rect==p_region_rect)
		return;

	region_rect=p_region_rect;

	item_rect_changed();
	_change_notify("region_rect");
}

Rect2 NinePatchRect::get_region_rect() const {

	return region_rect;
}

void NinePatchRect::set_draw_center(bool p_draw) {

	draw_center=p_draw;
	update();
}

bool NinePatchRect::get_draw_center() const{

	return draw_center;
}

NinePatchRect::NinePatchRect() {


	margin[MARGIN_LEFT]=0;
	margin[MARGIN_RIGHT]=0;
	margin[MARGIN_BOTTOM]=0;
	margin[MARGIN_TOP]=0;

	set_mouse_filter(MOUSE_FILTER_IGNORE);
	draw_center=true;
}


NinePatchRect::~NinePatchRect()
{
}
