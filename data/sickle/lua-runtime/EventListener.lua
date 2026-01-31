-- EventListener interface.


local EventListener = {}

local function addListener(self, listener)
    table.insert(self.listeners, listener)
end

local function removeListener(self, listener)
    for i,l in ipairs(self.listeners) do
        if l == listener then
            if l.on_removed then l:on_removed() end
            table.remove(self.listeners, i)
        end
    end
end

local function clearListeners(self)
    for i,l in ipairs(self.listeners) do
        if l.on_removed then l:on_removed() end
        table.remove(self.listeners, i)
    end
end

function doEvent(self, event, args)
    local captured = false
    for _,listener in ipairs(self.listeners) do
        local fn = listener[event]
        if fn then
            captured = fn(listener, args) or captured
        end
    end
    return captured
end


function EventListener.construct(self)
    self.listeners = {}
end

function EventListener.inherit(metatable)
    metatable.addListener = addListener
    metatable.removeListener = removeListener
    metatable.clearListeners = clearListeners
    metatable.doEvent = doEvent
end

return EventListener
