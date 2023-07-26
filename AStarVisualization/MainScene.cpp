#include "MainScene.hpp"

std::shared_ptr<Entity> MainScene::createEditText(const std::string initialText, unsigned fontSize, float left, float top) {
	auto entity = m_entityManager->addEntity();
	auto cEdit = entity->addComponent<CText>(initialText, m_config.font, fontSize);
	cEdit->text.setFillColor(sf::Color::White);
	cEdit->states.transform.translate(left, top);
	entity->addComponent<CClickable>([]() {return; });
	return entity;
}
std::shared_ptr<Entity> MainScene::createLabel(const std::string initialText, unsigned fontSize, float left, float top, sf::Color color) {
	auto entity = m_entityManager->addEntity();
	auto cEdit = entity->addComponent<CText>(initialText, m_config.font, fontSize);
	cEdit->text.setFillColor(color);
	cEdit->states.transform.translate(left, top);
	cEdit->canEdit = false;
	entity->addComponent<CClickable>([]() {return; });
	return entity;
}
std::shared_ptr<Entity> MainScene::createButton(float left, float top, float width, float height, std::function<void()> listener) {
	auto entity = m_entityManager->addEntity();
	auto vertexArr = sf::VertexArray(sf::Quads, 4);
	float halfWidth = width / 2.f, halfHeight = height / 2.f;
	vertexArr[0].position = { -halfWidth, -halfHeight };
	vertexArr[1].position = { halfWidth, -halfHeight };
	vertexArr[2].position = { halfWidth, halfHeight };
	vertexArr[3].position = { -halfWidth, halfHeight };
	auto cShape = entity->addComponent<CShape>(vertexArr);
	cShape->states.transform.translate(left, top);
	entity->addComponent<CClickable>(listener);
	return entity;
}
std::shared_ptr<Entity> MainScene::createBlock(float left, float top, float width, float height, sf::Color color) {
	auto entity = m_entityManager->addEntity();
	auto vertexArr = sf::VertexArray(sf::Quads, 4);
	float halfWidth = width / 2.f, halfHeight = height / 2.f;
	vertexArr[0].position = { 0, 0 };
	vertexArr[1].position = { width, 0 };
	vertexArr[2].position = { width, height };
	vertexArr[3].position = { 0, height };
	vertexArr[0].color = color;
	vertexArr[1].color = color;
	vertexArr[2].color = color;
	vertexArr[3].color = color;
	auto cShape = entity->addComponent<CShape>(vertexArr);
	cShape->states.transform.translate(left, top);
	auto cBlock = entity->addComponent<CBlock>();

	entity->addComponent<CClickable>(
		// Listener1
		[cShape, cBlock, this]() {
			cBlock->isObstacle = true;
			size_t size = cShape->vertexArray.getVertexCount();
			for (size_t i = 0; i < size; ++i) {
				cShape->vertexArray[i].color = obstacleColor;
			}},
		// Listener2
		[cShape, cBlock, this]() {
			cBlock->isObstacle = false;
			size_t size = cShape->vertexArray.getVertexCount();
			for (size_t i = 0; i < size; ++i) {
				cShape->vertexArray[i].color = pathColor;
			}}
	);
	return entity;
}
void MainScene::updateNM() {
	auto nText = nField->getComponent<CText>();
	auto mText = mField->getComponent<CText>();
	this->n = std::stoi(std::string(nText->text.getString()));
	this->m = std::stoi(std::string(mText->text.getString()));
	// Minimum value is 5
	if (this->n < 5) {
		nText->text.setString("5");
		this->n = 5;
	}
	if (this->m < 5) {
		mText->text.setString("5");
		this->m = 5;
	}

}
void MainScene::setColor(sf::VertexArray& array, sf::Color color) {
	size_t vertexSize = array.getVertexCount();
	for (size_t i = 0; i < vertexSize; ++i)
		array[i].color = color;
}

void MainScene::runAStar() {

	if (!AStarStarted) {
		AStarStarted = true;
		setColor(startButton->getComponent<CShape>()->vertexArray, grayColor);
		float scaler{ guidanceScaler };
		AStar<Vertex>::shortestPath(graph, startM + m * startN, endM + m * endN,
			[scaler](const std::pair<float, float>& posA, const std::pair<float, float>& posB) {
				// Calculate the squared euclidian distance -> quite greedy
				return scaler * (std::sqrt(powf(posA.first - posB.first, 2) + powf(posA.second - posB.second, 2)));
			});
		
	}
}


void MainScene::resetBlocks() {
	// Clear blocks
	for (auto& block : blocks) {
		block->destroy();
	}
	blocks.clear();
	// Creeate a graph with n*m vertices
	graph = AdjacencyListGraph<Vertex>(n * m);
	// Create blocks (570, 10) -> (1270, 710) // 700 X 700
	float size = std::min(700.f / n, 700.f / m);
	float halfSize = (size - 1) / 2.f;
	blocks.reserve(n * m);
	std::vector<std::pair<std::array<float, 2>, std::shared_ptr<Entity>>> pointBlockPairs;
	for (size_t i = 0; i < n; ++i) {
		for (size_t j = 0; j < m; ++j) {
			// Create a block & push them
			float x = 570 + j * size, y = 10 + i * size;
			auto blockEntity = createBlock(x, y, size - 1, size - 1, pathColor);
			blocks.push_back(blockEntity);
			pointBlockPairs.push_back({ { x + halfSize , y + halfSize }, blockEntity });

			// Add edges (from, to, weight) to the graph to form a n * m grid
			size_t cur = j + m * i;
			if (j != 0)
				graph.addEdge(cur, cur - 1, 1);
			if (j != m - 1)
				graph.addEdge(cur, cur + 1, 1);
			if (i != 0)
				graph.addEdge(cur, cur - m, 1);
			if (i != n - 1)
				graph.addEdge(cur, cur + m, 1);
			graph.getVertexAttribute(cur).pos = { i, j };
			graph.getVertexAttribute(cur).block = blockEntity;
		}
	}
	// Build a K-D tree to efficiently search blocks
	tree.buildTree(pointBlockPairs);
	// Set grid Range
	gridRect.height = n * size;
	gridRect.width = m * size;
	startN = std::floorf(n * 0.15f);
	startM = std::floorf(m * 0.15f);
	endN = std::floorf(n * 0.85f);
	endM = std::floorf(m * 0.85f);

	// Set start and end blocks
	float startH = gridRect.top + startN * size;
	float startW = gridRect.left + startM * size;
	float endH = gridRect.top + endN * size;
	float endW = gridRect.left + endM * size;

	auto startBlock = tree.findNearestNeighbor({ startW, startH }).second;
	startBlock->getComponent<CBlock>()->isStart = true;
	setColor(startBlock->getComponent<CShape>()->vertexArray, startColor);

	auto endBlock = tree.findNearestNeighbor({ endW, endH }).second;
	endBlock->getComponent<CBlock>()->isEnd = true;
	setColor(endBlock->getComponent<CShape>()->vertexArray, endColor);

	// Enable path calculation
	AStarStarted = false;
	if (startButton)
		setColor(startButton->getComponent<CShape>()->vertexArray, sf::Color::White);

}

void MainScene::init() {
	int fieldLeft = 100, fieldTop = 100;
	// nField
	nField = createEditText(std::to_string(n), 36, fieldLeft + 50, fieldTop);
	// mField
	mField = createEditText(std::to_string(m), 36, fieldLeft + 50, fieldTop + 50);
	// nLabel
	createLabel("N:", 36, fieldLeft, fieldTop);
	// mLabel
	createLabel("M:", 36, fieldLeft, fieldTop + 50);
	// Create Blocks
	resetBlocks();
	// Create reset button
	resetButton = createButton(fieldLeft + 50, fieldTop + 150, 100, 50, [this]() {
		updateNM();
		resetBlocks();
		});
	// Create reset button label
	createLabel("Reset", 36, fieldLeft, fieldTop + 125, sf::Color::Blue);
	// Create start button
	startButton = createButton(fieldLeft + 50, fieldTop + 225, 100, 50, [this]() {
		runAStar();
		});
	// Create start button label
	createLabel("Start", 36, fieldLeft + 10, fieldTop + 200, sf::Color::Red);
}

void MainScene::handleMouseInput(sf::Event& event) {

	if (event.type == sf::Event::MouseButtonPressed) {
		isMousePressing = true;
		if (event.mouseButton.button == sf::Mouse::Left) {
			leftPressing = true;
			float mouseX = event.mouseButton.x;
			float mouseY = event.mouseButton.y;
			// Check text fields
			for (auto& entity : { nField, mField }) {
				auto cText = entity->getComponent<CText>();
				// Reset focus
				cText->focused = false;
				auto& text = cText->text;
				text.setStyle(sf::Text::Style::Regular);
				// Set focus
				auto bound = cText->states.transform.transformRect(text.getGlobalBounds());
				if (bound.contains(mouseX, mouseY)) {
					cText->focused = true;
					text.setStyle(sf::Text::Style::Underlined);
				}
			}
			// Check buttons
			for (auto& entity : { resetButton, startButton }) {
				auto cShape = entity->getComponent<CShape>();
				auto bound = cShape->states.transform.transformRect(cShape->vertexArray.getBounds());
				auto cClick = entity->getComponent<CClickable>();
				if (cClick->isActive && bound.contains(mouseX, mouseY)) {
					cClick->onClickListener();
				}
			}
		}
		else {
			leftPressing = false;
		}
	}
	else if (event.type == sf::Event::MouseButtonReleased)
		isMousePressing = false;


}

void MainScene::handleKeyBoardInput(sf::Event& event) {
	if (event.type == sf::Event::TextEntered) {
		// Edit focused text
		for (auto& entity : { nField, mField }) {
			auto cText = entity->getComponent<CText>();
			auto& text = cText->text;
			auto& str = text.getString();
			auto code = event.text.unicode;
			if (cText->focused) {
				// backspace
				if (code == 8) {
					size_t size = str.getSize();
					if (size <= 1)
						text.setString("0");
					else
						text.setString(str.substring(0, size - 1));
				}
				// number
				else if (48 <= code && code <= 57) {
					size_t size = str.getSize();
					if (str == "0")
						text.setString(static_cast<char>(code));
					else
						text.setString(str + static_cast<char>(code));
				}
			}
		}
	}
}

void MainScene::update(sf::RenderWindow& window) {
	// Change the color of the block under the mouse cursor
	if (isMousePressing) {
		auto mousePos = sf::Mouse::getPosition(window);
		float mouseX = mousePos.x;
		float mouseY = mousePos.y;
		if (gridRect.contains(mouseX, mouseY)) {
			auto nearestButton = tree.findNearestNeighbor({ mouseX, mouseY }).second;
			auto cClick = nearestButton->getComponent<CClickable>();
			auto cBlock = nearestButton->getComponent<CBlock>();
			if (cClick->isActive && !cBlock->isStart && !cBlock->isEnd) {
				if (leftPressing)
					cClick->onClickListener();
				else
					cClick->additionalListener();
			}

		}
	}
}