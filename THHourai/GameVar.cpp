using namespace Touhou;

int GameVar::pickedCharacterType = -1;
RefPtr<GameObject> GameVar::playableCharacter;

void GameVar::Dispose( object sender )
{
	playableCharacter = nullptr;
}