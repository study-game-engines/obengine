HELLO_WORLD_GO_SCRIPT = [[---@class SampleObject : GameObjectCls
local SampleObject = GameObject();

local canvas;

function SampleObject:init()
    local screen_size = Engine.Window:get_size();
    canvas = obe.canvas.Canvas(screen_size.x, screen_size.y);

    canvas:Text "hello" {
        text = "Hello, world!",
        size = 128,
        color = obe.graphics.Color.White,
        x = 0.5,
        y = 0.5,
        unit = obe.transform.Units.ViewPercentage,
        align = {
            horizontal = "Center",
            vertical = "Center"
        }
    };
end

function Event.Game.Render()
    canvas:render(SampleObject.components.Sprite);
end
]]

HELLO_WORLD_GO_DEF = [[
Sprite:
    rect:
        x: 0.0
        y: 0.0
        width: 1.0
        height: 1.0
        unit: "ViewPercentage"
Script:
    source: "self://SampleObject.lua"
]]

HELLO_WORLD_SCENE = [[Meta:
    name: "SampleScene"

View:
    size: 1.0
    position:
        x: 0.0
        y: 0.0
        unit: SceneUnits
    referential: TopLeft

GameObjects:
    sampleGameObject:
        type: "SampleObject"
]]

HELLO_WORLD_BOOT = [[function Game.Start()
    Engine.Scene:loadFromFile("scenes://sample.map.vili");
end
]]

return {
    HELLO_WORLD_BOOT = HELLO_WORLD_BOOT,
    HELLO_WORLD_GO_DEF = HELLO_WORLD_GO_DEF,
    HELLO_WORLD_GO_SCRIPT = HELLO_WORLD_GO_SCRIPT,
    HELLO_WORLD_SCENE = HELLO_WORLD_SCENE
}
