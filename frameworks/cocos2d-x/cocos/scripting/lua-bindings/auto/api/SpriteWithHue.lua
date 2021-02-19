
--------------------------------
-- @module SpriteWithHue
-- @extend Sprite
-- @parent_module rad

--------------------------------
-- @overload self, cc.Texture2D, rect_table         
-- @overload self, cc.Texture2D         
-- @overload self, cc.Texture2D, rect_table, bool         
-- @function [parent=#SpriteWithHue] initWithTexture
-- @param self
-- @param #cc.Texture2D texture
-- @param #rect_table rect
-- @param #bool rotated
-- @return bool#bool ret (return value: bool)

--------------------------------
-- 
-- @function [parent=#SpriteWithHue] setupDefaultSettings 
-- @param self
-- @return SpriteWithHue#SpriteWithHue self (return value: SpriteWithHue)
        
--------------------------------
-- 
-- @function [parent=#SpriteWithHue] getHue 
-- @param self
-- @return float#float ret (return value: float)
        
--------------------------------
-- 
-- @function [parent=#SpriteWithHue] initWithSpriteFrame 
-- @param self
-- @param #cc.SpriteFrame spriteFrame
-- @return bool#bool ret (return value: bool)
        
--------------------------------
-- 
-- @function [parent=#SpriteWithHue] shaderBody 
-- @param self
-- @return char#char ret (return value: char)
        
--------------------------------
-- 
-- @function [parent=#SpriteWithHue] initShader 
-- @param self
-- @return SpriteWithHue#SpriteWithHue self (return value: SpriteWithHue)
        
--------------------------------
-- 
-- @function [parent=#SpriteWithHue] getAlpha 
-- @param self
-- @return float#float ret (return value: float)
        
--------------------------------
-- 
-- @function [parent=#SpriteWithHue] setHue 
-- @param self
-- @param #float hue
-- @return SpriteWithHue#SpriteWithHue self (return value: SpriteWithHue)
        
--------------------------------
-- 
-- @function [parent=#SpriteWithHue] hueUniformCallback 
-- @param self
-- @param #cc.GLProgram p
-- @param #cc.Uniform u
-- @return SpriteWithHue#SpriteWithHue self (return value: SpriteWithHue)
        
--------------------------------
-- @overload self, string, rect_table         
-- @overload self, string         
-- @function [parent=#SpriteWithHue] create
-- @param self
-- @param #string filename
-- @param #rect_table rect
-- @return SpriteWithHue#SpriteWithHue ret (return value: SpriteWithHue)

--------------------------------
-- @overload self, cc.Texture2D, rect_table, bool         
-- @overload self, cc.Texture2D         
-- @function [parent=#SpriteWithHue] createWithTexture
-- @param self
-- @param #cc.Texture2D texture
-- @param #rect_table rect
-- @param #bool rotated
-- @return SpriteWithHue#SpriteWithHue ret (return value: SpriteWithHue)

--------------------------------
-- 
-- @function [parent=#SpriteWithHue] createWithSpriteFrameName 
-- @param self
-- @param #string spriteFrameName
-- @return SpriteWithHue#SpriteWithHue ret (return value: SpriteWithHue)
        
--------------------------------
-- 
-- @function [parent=#SpriteWithHue] createWithSpriteFrame 
-- @param self
-- @param #cc.SpriteFrame spriteFrame
-- @return SpriteWithHue#SpriteWithHue ret (return value: SpriteWithHue)
        
return nil
