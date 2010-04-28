
-- game framework

dofile (package.path.."include/global.lua")

-- for card games
card_values = { "2", "3", "4", "5", "6", "7", "8", "9", "Jack", "Queen", "King", "Ace" }
card_suits = { "♠", "♥", "♦", "♣" }

----------------------------------------------------------------------------------------------------
--[[ user management
	 implemented functions:
		* create: create a user_list "object"
		* reset: resets the user list (removes all users)
		* add(user): add user to the list (returns true if successful, false if not)
		* remove(user): removes user from the list (returns true if successful, false if not)
		* contains(user): returns true if user is part of the list, false if not
		* size: returns the amount of users inside the list
		* get(index): returns user at index #index
		* concat: returns a (concatenated) string of all users
]]--
user_list = {}

user_list.__index = user_list
function user_list.create()
	local ul = {}
	setmetatable(ul, user_list)
	ul.list = {}
	return ul
end

function user_list:reset()
	self.list = {}
end

function user_list:add(user)
	if table.contains(self.list, user) then
		return false
	end
	
	table.insert(self.list, user)
	return true
end

function user_list:remove(user)
	if not table.contains(self.list, user) then
		return false
	end
	
	table.remove_elem(self.list, user)
	return true
end

function user_list:size()
	return table.maxn(self.list)
end

function user_list:get(index)
	return self.list[index]
end

function user_list:contains(index)
	return table.contains(self.list, index)
end

function user_list:concat()
	return table.concat(self.list, ", ")
end
----------------------------------------------------------------------------------------------------
